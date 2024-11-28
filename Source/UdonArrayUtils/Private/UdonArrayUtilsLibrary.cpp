// Copyright 2024 Udon-Tobira, All Rights Reserved.

#include "UdonArrayUtilsLibrary.h"

#include "Misc/EngineVersionComparison.h"

#include <algorithm>
#include <iterator>
#include <optional>
#include <random>
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
	    : target_ptr(InTargetPtr), property(InElementProperty) {}

public:
	// conversion operator to inner target_ptr
	[[nodiscard]] operator const void* const() const noexcept {
		return target_ptr;
	}

public:
	// equal operator
	[[nodiscard]] bool
	    operator==(const const_memory_transparent_reference& other) const {
		// if properties are different
		if (!property.SameType(&other.property)) {
			// return false
			return false;
		}

		// compare elements
		return property.Identical(target_ptr, other.target_ptr);
	}

	// equal operator with void* (without property check)
	[[nodiscard]] bool operator==(const void* const other) const {
		// compare values
		return property.Identical(target_ptr, other);
	}

	// not equal operator
	[[nodiscard]] bool
	    operator!=(const const_memory_transparent_reference& other) const {
		return !(*this == other);
	}

	// not equal operator with void* (without property check)
	[[nodiscard]] bool operator!=(const void* const other) const {
		return !(*this == other);
	}

public:
	const void* const target_ptr;
	const FProperty&  property;
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
	          new_target_ptr = ::operator new(other.property.GetSize()),
	          other.property) {
		// get memory size
		const auto& mem_size = property.GetSize();

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
	                                   other.property) {}

public:
	// conversion operator to inner target_ptr
	[[nodiscard]] operator void* const() const noexcept {
		return const_cast<void*>(target_ptr);
	}

public:
	// copy assignment operator
	memory_transparent_reference&
	    operator=(const memory_transparent_reference& other) {
		return *this =
		           static_cast<const const_memory_transparent_reference&>(other);
	}

	// copy assignment from const_memory_transparent_reference
	memory_transparent_reference&
	    operator=(const const_memory_transparent_reference& other) {
		// if properties are different
		if (!property.SameType(&other.property)) {
			// throw exception
			throw std::invalid_argument("property of this and other is different");
		}

		// get size
		const auto& mem_size = property.GetSize();

		// check size
		check(other.property.GetSize() == mem_size);

		// copy value
		std::memcpy(const_cast<void*>(target_ptr), other.target_ptr, mem_size);

		// return self
		return *this;
	}

	// copy assignment from void* (without property check)
	memory_transparent_reference& operator=(const void* const other) {
		// copy value
		std::memcpy(const_cast<void*>(target_ptr), other, property.GetSize());

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
		if (!a.property.SameType(&b.property)) {
			// throw exception
			throw std::invalid_argument("properties are different from each other");
		}

		// get size
		const auto& mem_size = a.property.GetSize();

		// check size
		check(b.property.GetSize() == mem_size);

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

	// arrow operator
	[[nodiscard]] pointer operator->() const noexcept {
		return &**this;
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
		// rebuild ref as needed and explitly ignore the [[nodiscard]] return value
		static_cast<void>(ScriptArrayHelperConstIterator::operator*());

		// return memory_transparent_reference
		return const_cast<ScriptArrayHelperIterator*>(this)->ref.value();
	}

	// arrow operator
	[[nodiscard]] pointer operator->() const noexcept {
		return &**this;
	}

	// prefix increment operator
	ScriptArrayHelperIterator& operator++() noexcept {
		ScriptArrayHelperConstIterator::operator++();
		return *this;
	}

	// postfix increment operator
	[[nodiscard]] ScriptArrayHelperIterator operator++(int) noexcept {
		auto tmp = *this;
		++*this;
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
		--*this;
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
 * Template to create constant false. Use this if I want to static_assert in
 * constexpr if statement.
 */
template <typename T>
constexpr bool false_v = false;

/**
 * Helper function to call a predicate function.
 */
template <class ReturnT, class... ArgTs>
static constexpr auto CreateLambdaToCallUFunction(UObject&    Context,
                                                  UFunction&  Predicate,
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
	return [&Context, &Predicate, ElementSize,
	        PredParamWorkingMemory =
	            std::move(PredicateParamWorkingMemory)](ArgTs... args) {
		// working offset on PredicateParamWorkingMemory
		size_t working_offset = 0;

		(..., [&]() {
			// if the ArgT is derived from const_memory_transparent_reference
			if constexpr (std::is_base_of_v<const_memory_transparent_reference,
			                                std::decay_t<ArgTs>>) {
				// check size
				check(ElementSize == args.property.GetSize());

				// get raw pointer
				const auto* const ptr = args.target_ptr;

				// copy data to PredParam
				std::memcpy(static_cast<uint8*>(PredParamWorkingMemory.get()) +
				                working_offset,
				            ptr, ElementSize);

				// add ElementSize to working_offset
				working_offset += ElementSize;
			} else {
				static_assert(udon::false_v<ArgTs>, "not implemented");
			}
		}());

		// call Predicate
		Context.ProcessEvent(&Predicate, PredParamWorkingMemory.get());

		// if return value is present
		if constexpr (!std::is_same_v<void, ReturnT>) {
			// return the result
			return *reinterpret_cast<ReturnT*>(
			    static_cast<uint8*>(PredParamWorkingMemory.get()) + working_offset);
		}
	};
}

inline int32 GetFPropertyElementSize(const FProperty& property) {
	return
#if UE_VERSION_OLDER_THAN(5, 5, 0)
	    property.ElementSize
#else
	    property.GetElementSize()
#endif
	    ;
}

class FScriptArrayBackInsertIterator {
public:
	using iterator_category = std::output_iterator_tag;
	using value_type        = void;
	using pointer           = void;
	using reference         = void;

	using container_type = FScriptArray;

	using difference_type = void;

public:
	FScriptArrayBackInsertIterator(FScriptArray&    InScriptArray,
	                               const FProperty& InElementProperty) noexcept
	    : ScriptArray(&InScriptArray), ElementProperty(&InElementProperty) {}

	FScriptArrayBackInsertIterator& operator=(const void* Value) {
		// create helper for ScriptArray
		auto ScriptArrayHelper = FScriptArrayHelper::CreateHelperFormInnerProperty(
		    ElementProperty, ScriptArray);

		// add an element
		ScriptArrayHelper.AddUninitializedValue();

		// get a pointer to the added element
		auto* const LastElementPtr =
		    ScriptArrayHelper.GetRawPtr(ScriptArrayHelper.Num() - 1);

		// get element size
		const auto& ElementSize = GetFPropertyElementSize(*ElementProperty);

		// copy Value to the added element
		std::memcpy(LastElementPtr, Value, ElementSize);

		return *this;
	}

	[[nodiscard]] FScriptArrayBackInsertIterator& operator*() noexcept {
		return *this;
	}

	FScriptArrayBackInsertIterator& operator++() noexcept {
		return *this;
	}

	FScriptArrayBackInsertIterator operator++(int) noexcept {
		return *this;
	}

protected:
	FScriptArray*    ScriptArray;
	const FProperty* ElementProperty;
};
} // namespace udon

/**
 * Get ArrayHelper, NumArray, ElementProperty, ElementSize, cbegin_it,
 * cend_it, begin_it, end_it.
 */
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
	const auto& ElementSize = GetFPropertyElementSize(*ElementProperty);         \
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
    UObject& Object, UFunction& BinaryPredicate) {
	PROCESS_ARRAY_ARGUMENTS();

	// Find the first iterator that satisfy BinaryPredicate
	const auto found_it = std::adjacent_find(
	    cbegin_it, cend_it,
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&,
	                                const const_memory_transparent_reference&>(
	        Object, BinaryPredicate, ElementSize));

	return found_it < cend_it ? std::distance(cbegin_it, found_it) : INDEX_NONE;
}

bool UUdonArrayUtilsLibrary::GenericAllSatisfy(
    const void* const TargetArray, const FArrayProperty& ArrayProperty,
    UObject& Object, UFunction& Predicate) {
	PROCESS_ARRAY_ARGUMENTS();

	// Check if all elements of TargetArray satisfy Predicate
	const auto bIsAllSatisfy = std::all_of(
	    cbegin_it, cend_it,
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&>(
	        Object, Predicate, ElementSize));

	return bIsAllSatisfy;
}

bool UUdonArrayUtilsLibrary::GenericAnySatisfy(
    const void* const TargetArray, const FArrayProperty& ArrayProperty,
    UObject& Object, UFunction& Predicate) {
	PROCESS_ARRAY_ARGUMENTS();

	// Check if any element of TargetArray satisfies Predicate
	const auto bIsAnySatisfy = std::any_of(
	    cbegin_it, cend_it,
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&>(
	        Object, Predicate, ElementSize));

	return bIsAnySatisfy;
}

int32 UUdonArrayUtilsLibrary::GenericCount(const void* const     TargetArray,
                                           const FArrayProperty& ArrayProperty,
                                           const void* const     ItemToCount) {
	PROCESS_ARRAY_ARGUMENTS();

	// Count the number of elements matching ItemToCount
	return std::count(cbegin_it, cend_it, ItemToCount);
}

int32 UUdonArrayUtilsLibrary::GenericCountIf(
    const void* const TargetArray, const FArrayProperty& ArrayProperty,
    UObject& Object, UFunction& Predicate) {
	PROCESS_ARRAY_ARGUMENTS();

	// Check if any element of TargetArray satisfies Predicate
	const auto bCount = std::count_if(
	    cbegin_it, cend_it,
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&>(
	        Object, Predicate, ElementSize));

	return bCount;
}

void UUdonArrayUtilsLibrary::GenericFill(void*                 TargetArray,
                                         const FArrayProperty& ArrayProperty,
                                         const void*           Value) {
	PROCESS_ARRAY_ARGUMENTS();

	// Fill the all elements of TargetArray with Value
	std::fill(begin_it, end_it, Value);
}

void UUdonArrayUtilsLibrary::GenericFill(void* const           TargetArray,
                                         const FArrayProperty& ArrayProperty,
                                         const int32           StartIndex,
                                         const int32           EndIndex,
                                         const void* const     Value) {
	PROCESS_ARRAY_ARGUMENTS();

	// Fill the elements of TargetArray with Value
	std::fill(begin_it + StartIndex, begin_it + EndIndex, Value);
}

int32 UUdonArrayUtilsLibrary::GenericFindIf(const void*           TargetArray,
                                            const FArrayProperty& ArrayProperty,
                                            UObject&              Object,
                                            UFunction&            Predicate) {
	PROCESS_ARRAY_ARGUMENTS();

	// Find the first iterator that satisfies Predicate
	const auto found_it = std::find_if(
	    cbegin_it, cend_it,
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&>(
	        Object, Predicate, ElementSize));

	return found_it < cend_it ? std::distance(cbegin_it, found_it) : INDEX_NONE;
}

const void* UUdonArrayUtilsLibrary::GenericMax(
    const void* TargetArray, const FArrayProperty& ArrayProperty,
    UObject& Object, UFunction& ComparisonFunction) {
	PROCESS_ARRAY_ARGUMENTS();

	// Find the max element's index
	const auto max_elem_index = GenericMaxElementIndex(
	    TargetArray, ArrayProperty, Object, ComparisonFunction);

	return INDEX_NONE == max_elem_index ? nullptr
	                                    : ArrayHelper.GetRawPtr(max_elem_index);
}

int32 UUdonArrayUtilsLibrary::GenericMaxElementIndex(
    const void* TargetArray, const FArrayProperty& ArrayProperty,
    UObject& Object, UFunction& ComparisonFunction) {
	PROCESS_ARRAY_ARGUMENTS();

	// Find the max element
	const auto max_it = std::max_element(
	    cbegin_it, cend_it,
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&,
	                                const const_memory_transparent_reference&>(
	        Object, ComparisonFunction, ElementSize));

	return max_it < cend_it ? std::distance(cbegin_it, max_it) : INDEX_NONE;
}

const void* UUdonArrayUtilsLibrary::GenericMin(
    const void* TargetArray, const FArrayProperty& ArrayProperty,
    UObject& Object, UFunction& ComparisonFunction) {
	PROCESS_ARRAY_ARGUMENTS();

	// Find the min element's index
	const auto min_elem_index = GenericMinElementIndex(
	    TargetArray, ArrayProperty, Object, ComparisonFunction);

	return INDEX_NONE == min_elem_index ? nullptr
	                                    : ArrayHelper.GetRawPtr(min_elem_index);
}

int32 UUdonArrayUtilsLibrary::GenericMinElementIndex(
    const void* TargetArray, const FArrayProperty& ArrayProperty,
    UObject& Object, UFunction& ComparisonFunction) {
	PROCESS_ARRAY_ARGUMENTS();

	// Find the min element
	const auto min_it = std::min_element(
	    cbegin_it, cend_it,
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&,
	                                const const_memory_transparent_reference&>(
	        Object, ComparisonFunction, ElementSize));

	return min_it < cend_it ? std::distance(cbegin_it, min_it) : INDEX_NONE;
}

bool UUdonArrayUtilsLibrary::GenericNoneSatisfy(
    const void* TargetArray, const FArrayProperty& ArrayProperty,
    UObject& Object, UFunction& Predicate) {
	return !GenericAnySatisfy(TargetArray, ArrayProperty, Object, Predicate);
}

void UUdonArrayUtilsLibrary::GenericRemoveRange(
    void* TargetArray, const FArrayProperty& ArrayProperty, int32 StartIndex,
    int32 EndIndex) {
	PROCESS_ARRAY_ARGUMENTS();

	// get number of elements to remove
	const auto& NumToRemove = EndIndex - StartIndex;

	// if there is nothing to remove
	if (NumToRemove == 0) {
		// finish
		return;
	}

	// remove elements in the range [StartIndex, EndIndex)
	ArrayHelper.RemoveValues(StartIndex, NumToRemove);
}

void UUdonArrayUtilsLibrary::GenericRemoveIf(
    void* TargetArray, const FArrayProperty& ArrayProperty, UObject& Object,
    UFunction& Predicate) {
	PROCESS_ARRAY_ARGUMENTS();

	// create lambda to call Predicate
	const auto lambda_predicate =
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&>(
	        Object, Predicate, ElementSize);

	// remove elements that satisfy Predicate
	for (auto i = decltype(NumArray){0}; i < ArrayHelper.Num(); ++i) {
		// get element as const_memory_transparent_reference
		const auto& elem_trans_ref = const_memory_transparent_reference(
		    ArrayHelper.GetRawPtr(i), *ElementProperty);

		// if the element satisfies Predicate
		if (lambda_predicate(elem_trans_ref)) {
			// remove the element
			ArrayHelper.RemoveValues(i, 1);
			--i;
		}
	}
}

std::pair<std::shared_ptr<FScriptArray>, std::shared_ptr<FScriptArray>>
    UUdonArrayUtilsLibrary::GenericRandomSample(
        const void* TargetArray, const FArrayProperty& ArrayProperty,
        int32 NumOfSamples) {
	PROCESS_ARRAY_ARGUMENTS();

	// create a new array to store samples
	auto Samples = std::make_shared<FScriptArray>();

	// create a new array to store others
	auto Others = std::make_shared<FScriptArray>();

	// if array is empty
	if (0 == NumArray) {
		// return the pair of empty arrays
		return {Samples, Others};
	}

	// create a back inserter for Samples
	FScriptArrayBackInsertIterator SamplesIt(*Samples, *ElementProperty);

	// create a back inserter for Others
	FScriptArrayBackInsertIterator OthersIt(*Others, *ElementProperty);

	// create a pseudo random source engine
	std::mt19937 mersenne_twister{std::random_device{}()};

	using unsigned_num_t = std::make_unsigned_t<std::decay_t<decltype(NumArray)>>;

	// sample elements
	unsigned_num_t rest_samples = NumOfSamples;
	for (auto it = cbegin_it; it < cend_it; ++it) {
		// get rest length of the array
		const auto& rest_length = std::distance(it, cend_it);

		// create a uniform distribution for rest_length
		std::uniform_int_distribution<unsigned_num_t> dist(0, rest_length - 1);

		// if the element is selected as a sample
		if (dist(mersenne_twister) < rest_samples) {
			// decrease the number of rest samples
			--rest_samples;

			// copy the element to Samples
			*SamplesIt = *it;

			// advance SamplesIt
			++SamplesIt;
		}
		// otherwise, the element is not selected as a sample
		else {
			// copy the element to Others
			*OthersIt = *it;

			// advance OthersIt
			++OthersIt;
		}
	}

	// return the pair of Samples and Others
	return {Samples, Others};
}

void UUdonArrayUtilsLibrary::GenericSortAnyArray(
    void* const TargetArray, const FArrayProperty& ArrayProperty,
    UObject& Object, UFunction& ComparisonFunction) {
	PROCESS_ARRAY_ARGUMENTS();

	// sort the elements of TargetArray
	std::sort(
	    begin_it, end_it,
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&,
	                                const const_memory_transparent_reference&>(
	        Object, ComparisonFunction, ElementSize));
}

#undef PROCESS_ARRAY_ARGUMENTS
