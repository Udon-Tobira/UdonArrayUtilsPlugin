// Copyright 2024 Udon-Tobira, All Rights Reserved.

#include "ArrayUtilsLibrary.h"

#include <algorithm>
#include <iterator>
#include <optional>
#include <stdexcept>

namespace udon {
/**
 * A class that only reads the type unknown value of a blueprint.
 */
class const_memory_transparent_reference {
public:
	// constructor
	const_memory_transparent_reference(
	    const void* const InTargetPtr,
	    const FProperty&  InElementProperty) noexcept
	    : target_ptr(InTargetPtr), elem_prop(InElementProperty) {}

public:
	// equal operator
	[[nodiscard]] bool
	    operator==(const const_memory_transparent_reference& other) const {
		// if properties are different
		if (!elem_prop.SameType(&other.elem_prop)) {
			// return false
			return false;
		}

		// compare elements
		return elem_prop.Identical(target_ptr, other.target_ptr);
	}

	// not equal operator
	[[nodiscard]] bool
	    operator!=(const const_memory_transparent_reference& other) const {
		return !(*this == other);
	}

public:
	const void* const target_ptr;
	const FProperty&  elem_prop;
};

/**
 * A class that can be used to swap memory areas for the actual target.
 * Swapping an instance of this class with the Swap function will swap the
 * contents in the actual memory.
 */
class memory_transparent_reference: public const_memory_transparent_reference {
public:
	// constructor
	memory_transparent_reference(void*            InTargetPtr,
	                             const FProperty& InElementProperty) noexcept
	    : const_memory_transparent_reference(InTargetPtr, InElementProperty),
	      new_target_ptr(nullptr) {}

	// copy constructor
	memory_transparent_reference(const memory_transparent_reference& other)
	    : const_memory_transparent_reference(
	          new_target_ptr = ::operator new(other.elem_prop.GetSize()),
	          other.elem_prop) {
		// get memory size
		const auto& mem_size = elem_prop.GetSize();

		// copy value
		std::memcpy(const_cast<void*>(target_ptr), other.target_ptr, mem_size);
	}

	// move constructor
	memory_transparent_reference(memory_transparent_reference&& other) noexcept
	    : memory_transparent_reference(other) {}

	// converting constructor from const_memory_transparent_reference
	explicit memory_transparent_reference(
	    const const_memory_transparent_reference& other)
	    : memory_transparent_reference(const_cast<void*>(other.target_ptr),
	                                   other.elem_prop) {}

public:
	// copy assignment operator
	memory_transparent_reference&
	    operator=(const memory_transparent_reference& other) {
		// if properties are different
		if (!elem_prop.SameType(&other.elem_prop)) {
			// throw exception
			throw std::invalid_argument("property of this and other is different");
		}

		// get size
		const auto& mem_size = elem_prop.GetSize();

		// check size
		check(other.elem_prop.GetSize() == mem_size);

		// copy value
		std::memcpy(const_cast<void*>(target_ptr), other.target_ptr, mem_size);

		// return self
		return *this;
	}

	// move assignment operator
	memory_transparent_reference&
	    operator=(memory_transparent_reference&& other) {
		// copy assign
		return *this = other;
	}

public:
	// swap
	friend void swap(memory_transparent_reference& a,
	                 memory_transparent_reference& b) {
		// if properties are different
		if (!a.elem_prop.SameType(&b.elem_prop)) {
			// throw exception
			throw std::invalid_argument("properties are different from each other");
		}

		// get size
		const auto& mem_size = a.elem_prop.GetSize();

		// check size
		check(b.elem_prop.GetSize() == mem_size);

		FMemory::Memswap(const_cast<void*>(a.target_ptr),
		                 const_cast<void*>(b.target_ptr), mem_size);
	}

public:
	// destructor
	~memory_transparent_reference() noexcept {
		// if this class allocates the memory of new_target_ptr
		if (new_target_ptr) {
			// delete newed memory
			::operator delete(new_target_ptr);
		}
	}

private:
	void* new_target_ptr;
};

class ScriptArrayHelperConstIterator {
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type        = const_memory_transparent_reference;
	using reference         = const value_type&;
	using difference_type   = int32;
	using pointer           = const value_type*;

public:
	// default constructor
	ScriptArrayHelperConstIterator() noexcept
	    : ArrayHelper(nullptr), ElementProperty(nullptr), index() {}

	// constructor
	ScriptArrayHelperConstIterator(FScriptArrayHelper&    InArrayHelper,
	                               const FProperty* const InElementProperty,
	                               const int32            in_index) noexcept
	    : ArrayHelper(&InArrayHelper), ElementProperty(InElementProperty),
	      index(in_index) {}

	// copy constructor
	ScriptArrayHelperConstIterator(
	    const ScriptArrayHelperConstIterator& other) noexcept
	    : ArrayHelper(other.ArrayHelper), ElementProperty(other.ElementProperty),
	      index(other.index) {}

public:
	// copy assignment operator
	ScriptArrayHelperConstIterator&
	    operator=(const ScriptArrayHelperConstIterator& other) noexcept {
		auto old_index = index;

		ArrayHelper     = other.ArrayHelper;
		ElementProperty = other.ElementProperty;
		index           = other.index;

		// if index changed
		if (old_index != index) {
			need_to_rebuild_ref = true;
		}

		return *this;
	}

public:
	// dereference operator
	[[nodiscard]] reference operator*() const noexcept {
		// if need to rebuild ref
		if (need_to_rebuild_ref) {
			// rebuild ref
			const_cast<ScriptArrayHelperConstIterator*>(this)->ref.emplace(
			    ArrayHelper->GetRawPtr(index), *ElementProperty);
		}

		// turn off need_to_rebuild_ref flag
		const_cast<ScriptArrayHelperConstIterator*>(this)->need_to_rebuild_ref =
		    false;

		// return const_memory_transparent_reference
		return ref.value();
	}

	// prefix increment operator
	ScriptArrayHelperConstIterator& operator++() noexcept {
		++index;
		need_to_rebuild_ref = true;
		return *this;
	}

	// postfix increment operator
	[[nodiscard]] ScriptArrayHelperConstIterator operator++(int) noexcept {
		auto tmp = *this;
		++*this;
		return tmp;
	}

	// prefix decrement operator
	ScriptArrayHelperConstIterator& operator--() noexcept {
		--index;
		need_to_rebuild_ref = true;
		return *this;
	}

	// postfix decrement operator
	[[nodiscard]] ScriptArrayHelperConstIterator operator--(int) noexcept {
		auto tmp = *this;
		--*this;
		return tmp;
	}

	// ScriptArrayHelperConstIterator += offset operator
	ScriptArrayHelperConstIterator&
	    operator+=(const difference_type offset) noexcept {
		index += offset;
		need_to_rebuild_ref = true;
		return *this;
	}

	// ScriptArrayHelperConstIterator + offset operator
	[[nodiscard]] ScriptArrayHelperConstIterator
	    operator+(const difference_type offset) const noexcept {
		auto tmp = *this;
		tmp += offset;
		return tmp;
	}

	// offset + ScriptArrayHelperConstIterator operator
	[[nodiscard]] friend ScriptArrayHelperConstIterator
	    operator+(const difference_type                 offset,
	              const ScriptArrayHelperConstIterator& other) noexcept {
		return other + offset;
	}

	// ScriptArrayHelperConstIterator -= offset operator
	ScriptArrayHelperConstIterator&
	    operator-=(const difference_type offset) noexcept {
		return *this += -offset;
	}

	// ScriptArrayHelperConstIterator - offset operator
	[[nodiscard]] ScriptArrayHelperConstIterator
	    operator-(const difference_type offset) const noexcept {
		return *this + (-offset);
	}

	// ScriptArrayHelperConstIterator - ScriptArrayHelperConstIterator operator
	[[nodiscard]] difference_type
	    operator-(const ScriptArrayHelperConstIterator& other) const noexcept {
		return index - other.index;
	}

	// [] operator
	[[nodiscard]] reference
	    operator[](const difference_type offset) const noexcept {
		return *(*this + offset);
	}

	// == operator
	[[nodiscard]] bool
	    operator==(const ScriptArrayHelperConstIterator& other) const noexcept {
		return index == other.index;
	}

	// != operator
	[[nodiscard]] bool
	    operator!=(const ScriptArrayHelperConstIterator& other) const noexcept {
		return !(*this == other);
	}

	// < operator
	[[nodiscard]] bool
	    operator<(const ScriptArrayHelperConstIterator& other) const noexcept {
		return index < other.index;
	}

	// > operator
	[[nodiscard]] bool
	    operator>(const ScriptArrayHelperConstIterator& other) const noexcept {
		return index > other.index;
	}

	// <= operator
	[[nodiscard]] bool
	    operator<=(const ScriptArrayHelperConstIterator& other) const noexcept {
		return !(*this > other);
	}

	// >= operator
	[[nodiscard]] bool
	    operator>=(const ScriptArrayHelperConstIterator& other) const noexcept {
		return !(*this < other);
	}

protected:
	FScriptArrayHelper*                         ArrayHelper;
	const FProperty*                            ElementProperty;
	int32                                       index;
	std::optional<memory_transparent_reference> ref;
	bool                                        need_to_rebuild_ref = true;
};
class ScriptArrayHelperIterator: public ScriptArrayHelperConstIterator {
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type        = memory_transparent_reference;
	using reference         = value_type&;
	using difference_type   = int32;
	using pointer           = value_type*;

public:
	// inheriting constructors
	using ScriptArrayHelperConstIterator::ScriptArrayHelperConstIterator;

public:
	// dereference operator
	[[nodiscard]] reference operator*() const noexcept {
		return const_cast<ScriptArrayHelperIterator*>(this)->ref.emplace(
		    ArrayHelper->GetRawPtr(index), *ElementProperty);
	}

	// prefix increment operator
	ScriptArrayHelperIterator& operator++() noexcept {
		ScriptArrayHelperConstIterator::operator++();
		return *this;
	}

	// postfix increment operator
	[[nodiscard]] ScriptArrayHelperIterator operator++(int) noexcept {
		auto tmp = *this;
		ScriptArrayHelperConstIterator::operator++();
		return tmp;
	}

	// prefix decrement operator
	ScriptArrayHelperIterator& operator--() noexcept {
		ScriptArrayHelperConstIterator::operator--();
		return *this;
	}

	// postfix decrement operator
	[[nodiscard]] ScriptArrayHelperIterator operator--(int) noexcept {
		auto tmp = *this;
		ScriptArrayHelperConstIterator::operator--();
		return tmp;
	}

	// ScriptArrayHelperIterator += offset operator
	ScriptArrayHelperIterator& operator+=(const difference_type offset) noexcept {
		ScriptArrayHelperConstIterator::operator+=(offset);
		return *this;
	}

	// ScriptArrayHelperIterator + offset operator
	[[nodiscard]] ScriptArrayHelperIterator
	    operator+(const difference_type offset) const noexcept {
		auto tmp = *this;
		tmp += offset;
		return tmp;
	}

	// offset + ScriptArrayHelperIterator operator
	[[nodiscard]] friend ScriptArrayHelperIterator
	    operator+(const difference_type            offset,
	              const ScriptArrayHelperIterator& other) noexcept {
		return other + offset;
	}

	// ScriptArrayHelperIterator -= offset operator
	ScriptArrayHelperIterator& operator-=(const difference_type offset) noexcept {
		ScriptArrayHelperConstIterator::operator-=(offset);
		return *this;
	}

	// ScriptArrayHelperIterator - ScriptArrayHelperIterator operator
	using ScriptArrayHelperConstIterator::operator-;

	// ScriptArrayHelperIterator - offset operator
	[[nodiscard]] ScriptArrayHelperIterator
	    operator-(const difference_type offset) const noexcept {
		auto tmp = *this;
		tmp -= offset;
		return tmp;
	}

	// [] operator
	[[nodiscard]] reference
	    operator[](const difference_type offset) const noexcept {
		return *(*this + offset);
	}

private:
	std::optional<value_type> ref;
};

ScriptArrayHelperIterator begin(FScriptArrayHelper&    ArrayHelper,
                                const FProperty* const ElementProperty) {
	return ScriptArrayHelperIterator(ArrayHelper, ElementProperty, 0);
}
ScriptArrayHelperIterator end(FScriptArrayHelper&    ArrayHelper,
                              const FProperty* const ElementProperty) {
	// get length of the array
	const auto& NumArray = ArrayHelper.Num();

	return ScriptArrayHelperIterator(ArrayHelper, ElementProperty, NumArray);
}
ScriptArrayHelperConstIterator cbegin(FScriptArrayHelper&    ArrayHelper,
                                      const FProperty* const ElementProperty) {
	return ScriptArrayHelperConstIterator(ArrayHelper, ElementProperty, 0);
}
ScriptArrayHelperConstIterator cend(FScriptArrayHelper&    ArrayHelper,
                                    const FProperty* const ElementProperty) {
	// get length of the array
	const auto& NumArray = ArrayHelper.Num();

	return ScriptArrayHelperConstIterator(ArrayHelper, ElementProperty, NumArray);
}

/**
 * Helper function to call a predicate function with one element.
 */
template <class ReturnT, class... ArgTs>
static constexpr auto CreateLambdaToCallUFunction(UFunction&  Predicate,
                                                  const int32 ElementSize) {
	// Total memory size of Predicate arguments and return value
	const auto& PredicateParamSize =
	    (... + (std::is_base_of_v<const_memory_transparent_reference,
	                              std::decay_t<ArgTs>>
	                ? ElementSize
	                : sizeof(ArgTs))) +
	    sizeof(ReturnT);

	// Working memory for Predicate parameters
	std::shared_ptr<void> PredicateParamWorkingMemory(
	    ::operator new(PredicateParamSize),
	    [](void* const ptr) { ::operator delete(ptr); });

	// retun lambda
	return [&Predicate, ElementSize,
	        PredParamWorkingMemory =
	            std::move(PredicateParamWorkingMemory)](ArgTs... args) {
		// working offset on PredicateParamWorkingMemory
		size_t working_offset = 0;

		(..., [&]() {
			// if the ArgT is derived from const_memory_transparent_reference
			if constexpr (std::is_base_of_v<const_memory_transparent_reference,
			                                std::decay_t<ArgTs>>) {
				// check size
				check(ElementSize == args.elem_prop.GetSize());

				// get raw pointer
				const auto* const ptr = args.target_ptr;

				// copy data to PredParam
				std::memcpy(static_cast<uint8*>(PredParamWorkingMemory.get()) +
				                working_offset,
				            ptr, ElementSize);

				// add ElementSize to working_offset
				working_offset += ElementSize;
			} else {
				static_assert(false, "not implemented");
			}
		}());

		// call Predicate
		Predicate.GetOuter()->ProcessEvent(&Predicate,
		                                   PredParamWorkingMemory.get());

		// if return value is present
		if constexpr (!std::is_same_v<void, ReturnT>) {
			// return the result
			return *reinterpret_cast<ReturnT*>(
			    static_cast<uint8*>(PredParamWorkingMemory.get()) + working_offset);
		}
	};
}
} // namespace udon

#define PROCESS_ARRAY_ARGUMENTS()                                              \
	using namespace udon;                                                        \
                                                                               \
	/* helper to allow access to the actual array 	*/                            \
	FScriptArrayHelper ArrayHelper(&ArrayProperty, TargetArray);                 \
                                                                               \
	/* get length of the array			*/                                              \
	const auto& NumArray = ArrayHelper.Num();                                    \
                                                                               \
	/* get property of the element	*/                                            \
	const auto* const ElementProperty = ArrayProperty.Inner;                     \
                                                                               \
	/* get the size of one element */                                            \
	const auto& ElementSize = ElementProperty->ElementSize;                      \
                                                                               \
	/* create the begin and end const iterators of the TargetArray */            \
	auto cbegin_it = cbegin(ArrayHelper, ElementProperty);                       \
	auto cend_it   = cend(ArrayHelper, ElementProperty);                         \
                                                                               \
	/* create the begin and end iterators of the TargetArray */                  \
	auto begin_it = begin(ArrayHelper, ElementProperty);                         \
	auto end_it   = end(ArrayHelper, ElementProperty);

int32 UUdonArrayUtilsLibrary::GenericAdjacentFind(
    const void* const TargetArray, const FArrayProperty& ArrayProperty,
    UFunction& BinaryPredicate) {
	PROCESS_ARRAY_ARGUMENTS();

	// Find the first iterator that satisfy BinaryPredicate
	auto found_it = std::adjacent_find(
	    cbegin_it, cend_it,
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&,
	                                const const_memory_transparent_reference&>(
	        BinaryPredicate, ElementSize));

	return found_it < cend_it ? std::distance(cbegin_it, found_it) : INDEX_NONE;
}

bool UUdonArrayUtilsLibrary::GenericAllSatisfy(
    const void* const TargetArray, const FArrayProperty& ArrayProperty,
    UFunction& Predicate) {
	PROCESS_ARRAY_ARGUMENTS();

	// Check if all elements of TargetArray satisfy Predicate
	auto bIsAllSatisfy = std::all_of(
	    cbegin_it, cend_it,
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&>(
	        Predicate, ElementSize));

	return bIsAllSatisfy;
}

bool UUdonArrayUtilsLibrary::GenericAnySatisfy(
    const void* const TargetArray, const FArrayProperty& ArrayProperty,
    UFunction& Predicate) {
	PROCESS_ARRAY_ARGUMENTS();

	// Check if any element of TargetArray satisfies Predicate
	auto bIsAnySatisfy = std::any_of(
	    cbegin_it, cend_it,
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&>(
	        Predicate, ElementSize));

	return bIsAnySatisfy;
}

int32 UUdonArrayUtilsLibrary::GenericCount(const void* const     TargetArray,
                                           const FArrayProperty& ArrayProperty,
                                           const void* const     ItemToCount) {
	PROCESS_ARRAY_ARGUMENTS();

	// Count the number of elements matching ItemToCount
	return std::count(
	    cbegin_it, cend_it,
	    const_memory_transparent_reference(ItemToCount, *ElementProperty));
}

int32 UUdonArrayUtilsLibrary::GenericCountIf(
    const void* TargetArray, const FArrayProperty& ArrayProperty,
    UFunction& Predicate) {
	PROCESS_ARRAY_ARGUMENTS();

	// Check if any element of TargetArray satisfies Predicate
	auto bCount = std::count_if(
	    cbegin_it, cend_it,
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&>(
	        Predicate, ElementSize));

	return bCount;
}

void UUdonArrayUtilsLibrary::GenericSortAnyArray(
    void* const TargetArray, const FArrayProperty& ArrayProperty,
    UFunction& ComparisonFunction) {
	PROCESS_ARRAY_ARGUMENTS();

	// sort the elements of TargetArray
	std::sort(
	    begin_it, end_it,
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&,
	                                const const_memory_transparent_reference&>(
	        ComparisonFunction, ElementSize));
}

#undef PROCESS_ARRAY_ARGUMENTS
