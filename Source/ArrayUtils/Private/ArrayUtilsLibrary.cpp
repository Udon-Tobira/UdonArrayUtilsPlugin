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
	[[nodiscard]] bool operator==(
	    const const_memory_transparent_reference& other) const noexcept {
		// if properties are different
		if (!elem_prop.SameType(&other.elem_prop)) {
			// return false
			return false;
		}

		// compare elements
		return elem_prop.Identical(target_ptr, other.target_ptr);
	}

	// not equal operator
	[[nodiscard]] bool operator!=(
	    const const_memory_transparent_reference& other) const noexcept {
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
	memory_transparent_reference(
	    const memory_transparent_reference& other) noexcept
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
	    : ArrayHelper(nullptr), ElemProp(nullptr), index() {}

	// constructor
	ScriptArrayHelperConstIterator(FScriptArrayHelper&    InArrayHelper,
	                               const FProperty* const in_element_property,
	                               const int32            in_index) noexcept
	    : ArrayHelper(&InArrayHelper), ElemProp(in_element_property),
	      index(in_index) {}

	// copy constructor
	ScriptArrayHelperConstIterator(
	    const ScriptArrayHelperConstIterator& other) noexcept
	    : ArrayHelper(other.ArrayHelper), ElemProp(other.ElemProp),
	      index(other.index) {}

public:
	// copy assignment operator
	ScriptArrayHelperConstIterator&
	    operator=(const ScriptArrayHelperConstIterator& other) noexcept {
		ArrayHelper = other.ArrayHelper;
		ElemProp    = other.ElemProp;
		index       = other.index;

		return *this;
	}

public:
	// dereference operator
	[[nodiscard]] reference operator*() const noexcept {
		return const_cast<ScriptArrayHelperConstIterator*>(this)->ref.emplace(
		    ArrayHelper->GetRawPtr(index), *ElemProp);
	}

	// prefix increment operator
	ScriptArrayHelperConstIterator& operator++() noexcept {
		++index;
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
	FScriptArrayHelper* ArrayHelper;
	const FProperty*    ElemProp;
	int32               index;

private:
	std::optional<value_type> ref;
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
		    ArrayHelper->GetRawPtr(index), *ElemProp);
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

/**
 * Helper function to call a predicate function with one element.
 */
template <class ReturnT, class... Args>
static constexpr auto
    CreateLambdaToCallUFunction(UFunction&  Predicate,
                                void* const PredParamWorkingMemory,
                                const int32 ElemSize) {
	return [&](Args... args) {
		size_t working_offset = 0;

		(..., [&]() {
			// if Args is derived from const_memory_transparent_reference
			if constexpr (std::is_base_of_v<const_memory_transparent_reference,
			                                std::decay_t<Args>>) {
				// check size
				check(ElemSize == args.elem_prop.GetSize());

				// get raw pointer
				const auto* const ptr = args.target_ptr;

				// copy data to PredParam
				std::memcpy(static_cast<uint8*>(PredParamWorkingMemory) +
				                working_offset,
				            ptr, ElemSize);

				// add ElemSize to working_offset
				working_offset += ElemSize;
			} else {
				static_assert(false, "not implemented");
			}
		}());

		// call Predicate
		Predicate.GetOuter()->ProcessEvent(&Predicate, PredParamWorkingMemory);

		// if return value is present
		if constexpr (!std::is_same_v<void, ReturnT>) {
			// return the result
			return *reinterpret_cast<ReturnT*>(
			    static_cast<uint8*>(PredParamWorkingMemory) + working_offset);
		}
	};
}
} // namespace udon

int32 UUdonArrayUtilsLibrary::GenericAdjacentFind(
    const void* const TargetArray, const FArrayProperty& ArrayProperty,
    UFunction& BinaryPredicate) {
	using namespace udon;

	// helper to allow access to the actual array
	FScriptArrayHelper ArrayHelper(&ArrayProperty, TargetArray);

	// get length of the array
	const auto& NumArray = ArrayHelper.Num();

	// get property of the element
	const auto* const ElemProp = ArrayProperty.Inner;

	// get the size of one element
	const auto& ElemSize = ElemProp->ElementSize;

	// allocate memory for function parameters
	// argument current, next (2 * ElemSize)
	// return value (sizeof(bool))
	void* const PredParam = ::operator new(ElemSize + ElemSize + sizeof(bool));

	// create the begin and end iterators of the TargetArray
	auto begin = ScriptArrayHelperConstIterator(ArrayHelper, ElemProp, 0);
	auto end   = ScriptArrayHelperConstIterator(ArrayHelper, ElemProp, NumArray);

	// Find the first iterator that satisfy BinaryPredicate
	auto found_it = std::adjacent_find(
	    begin, end,
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&,
	                                const const_memory_transparent_reference&>(
	        BinaryPredicate, PredParam, ElemSize));

	// delete memory
	::operator delete(PredParam);

	return found_it < end ? std::distance(begin, found_it) : INDEX_NONE;
}

bool UUdonArrayUtilsLibrary::GenericAllSatisfy(
    const void* const TargetArray, const FArrayProperty& ArrayProperty,
    UFunction& Predicate) {
	using namespace udon;

	// helper to allow access to the actual array
	FScriptArrayHelper ArrayHelper(&ArrayProperty, TargetArray);

	// get length of the array
	const auto& NumArray = ArrayHelper.Num();

	// get property of the element
	const auto* const ElemProp = ArrayProperty.Inner;

	// get the size of one element
	const auto& ElemSize = ElemProp->ElementSize;

	// allocate memory for function parameters
	// argument ElemSize
	// return value (sizeof(bool))
	void* const PredParam = ::operator new(ElemSize + sizeof(bool));

	// create the begin and end iterators of the TargetArray
	auto begin = ScriptArrayHelperConstIterator(ArrayHelper, ElemProp, 0);
	auto end   = ScriptArrayHelperConstIterator(ArrayHelper, ElemProp, NumArray);

	// Check if all elements of TargetArray satisfy Predicate
	auto bIsAllSatisfy = std::all_of(
	    begin, end,
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&>(
	        Predicate, PredParam, ElemSize));

	// delete memory
	::operator delete(PredParam);

	return bIsAllSatisfy;
}

bool UUdonArrayUtilsLibrary::GenericAnySatisfy(
    const void* const TargetArray, const FArrayProperty& ArrayProperty,
    UFunction& Predicate) {
	using namespace udon;

	// helper to allow access to the actual array
	FScriptArrayHelper ArrayHelper(&ArrayProperty, TargetArray);

	// get length of the array
	const auto& NumArray = ArrayHelper.Num();

	// get property of the element
	const auto* const ElemProp = ArrayProperty.Inner;

	// get the size of one element
	const auto& ElemSize = ElemProp->ElementSize;

	// allocate memory for function parameters
	// argument ElemSize
	// return value (sizeof(bool))
	void* const PredParam = ::operator new(ElemSize + sizeof(bool));

	// create the begin and end iterators of the TargetArray
	auto begin = ScriptArrayHelperConstIterator(ArrayHelper, ElemProp, 0);
	auto end   = ScriptArrayHelperConstIterator(ArrayHelper, ElemProp, NumArray);

	// Check if any element of TargetArray satisfies Predicate
	auto bIsAnySatisfy = std::any_of(
	    begin, end,
	    CreateLambdaToCallUFunction<bool,
	                                const const_memory_transparent_reference&>(
	        Predicate, PredParam, ElemSize));

	// delete memory
	::operator delete(PredParam);

	return bIsAnySatisfy;
}

int32 UUdonArrayUtilsLibrary::GenericCount(const void* const     TargetArray,
                                           const FArrayProperty& ArrayProperty,
                                           const void* const     ItemToCount) {
	using namespace udon;

	// helper to allow access to the actual array
	FScriptArrayHelper ArrayHelper(&ArrayProperty, TargetArray);

	// get length of the array
	const auto& NumArray = ArrayHelper.Num();

	// get property of the element
	const auto* const ElemProp = ArrayProperty.Inner;

	// get the size of one element
	const auto& ElemSize = ElemProp->ElementSize;

	// create the begin and end iterators of the TargetArray
	auto begin = ScriptArrayHelperConstIterator(ArrayHelper, ElemProp, 0);
	auto end   = ScriptArrayHelperConstIterator(ArrayHelper, ElemProp, NumArray);

	// Count the number of elements matching ItemToCount
	return std::count(begin, end,
	                  const_memory_transparent_reference(ItemToCount, *ElemProp));
}

void UUdonArrayUtilsLibrary::GenericSortAnyArray(
    void* const TargetArray, const FArrayProperty& ArrayProperty,
    UFunction& ComparisonFunction) {
	using namespace udon;

	// helper to allow manipulation of the actual array
	FScriptArrayHelper ArrayHelper(&ArrayProperty, TargetArray);

	// get length of the array
	const auto& NumArray = ArrayHelper.Num();

	// get property of the element
	const auto* const ElemProp = ArrayProperty.Inner;

	// get the size of one element
	const auto& ElemSize = ElemProp->ElementSize;

	// allocate memory for function parameters
	// argument A, B (2 * ElemSize)
	// return value (sizeof(bool))
	void* const ComparisonFunctionParam =
	    ::operator new(ElemSize + ElemSize + sizeof(bool));

	// create the begin and end iterators of the TargetArray
	auto begin = ScriptArrayHelperIterator(ArrayHelper, ElemProp, 0);
	auto end   = ScriptArrayHelperIterator(ArrayHelper, ElemProp, NumArray);

	// sort the elements of TargetArray
	std::sort(
	    begin, end,
	    CreateLambdaToCallUFunction<bool, const_memory_transparent_reference&,
	                                const_memory_transparent_reference&>(
	        ComparisonFunction, ComparisonFunctionParam, ElemSize));

	// delete memory
	::operator delete(ComparisonFunctionParam);
}
