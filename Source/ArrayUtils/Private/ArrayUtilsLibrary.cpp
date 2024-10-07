// Copyright 2024 Udon-Tobira, All Rights Reserved.

#include "ArrayUtilsLibrary.h"

#include <algorithm>
#include <iterator>
#include <optional>

namespace udon {
/**
 * A class that can be used to swap memory areas for the actual target.
 * Swapping an instance of this class with the Swap function will swap the
 * contents in the actual memory.
 */
class memory_transparent_reference {
public:
	// constructor
	memory_transparent_reference(void* InTargetPtr, int32 InSize) noexcept
	    : target_ptr(InTargetPtr), mem_size(InSize), newed_ptr(false) {}

	// copy constructor
	memory_transparent_reference(
	    const memory_transparent_reference& other) noexcept
	    : target_ptr(std::malloc(other.mem_size)), mem_size(other.mem_size),
	      newed_ptr(true) {
		// copy value
		std::memcpy(target_ptr, other.target_ptr, mem_size);
	}

	// move constructor
	memory_transparent_reference(memory_transparent_reference&& other) noexcept
	    : memory_transparent_reference(other) {}

public:
	// copy assignment operator
	memory_transparent_reference&
	    operator=(const memory_transparent_reference& other) {
		// if memory size is different
		if (mem_size != other.mem_size) {
			// throw exception
			throw std::invalid_argument("memory size of this and other is different");
		}

		// copy value
		std::memcpy(target_ptr, other.target_ptr, mem_size);

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
		// if memory size is different
		if (a.mem_size != b.mem_size) {
			// throw exception
			throw std::invalid_argument("memory sizes are different from each other");
		}

		FMemory::Memswap(a.target_ptr, b.target_ptr, a.mem_size);
	}

public:
	// destructor
	~memory_transparent_reference() noexcept {
		// if this class allocates the memory of target_ptr
		if (newed_ptr) {
			// free memory
			std::free(target_ptr);
		}
	}

public:
	void* target_ptr;
	int32 mem_size;
	bool  newed_ptr;
};

class ScriptArrayHelperConstIterator {
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type        = memory_transparent_reference;
	using reference         = const value_type&;
	using difference_type   = int32;
	using pointer           = const value_type*;

public:
	// default constructor
	ScriptArrayHelperConstIterator() noexcept
	    : ArrayHelper(nullptr), element_size(), index() {}

	// constructor
	ScriptArrayHelperConstIterator(FScriptArrayHelper& InArrayHelper,
	                               const int32         in_element_size,
	                               const int32         in_index) noexcept
	    : ArrayHelper(&InArrayHelper), element_size(in_element_size),
	      index(in_index) {}

	// copy constructor
	ScriptArrayHelperConstIterator(
	    const ScriptArrayHelperConstIterator& other) noexcept
	    : ArrayHelper(other.ArrayHelper), element_size(other.element_size),
	      index(other.index) {}

public:
	// dereference operator
	[[nodiscard]] reference operator*() const noexcept {
		return const_cast<ScriptArrayHelperConstIterator*>(this)->ref.emplace(
		    ArrayHelper->GetRawPtr(index), element_size);
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

private:
	std::optional<memory_transparent_reference> ref;
	FScriptArrayHelper*                         ArrayHelper;
	int32                                       element_size;
	int32                                       index;
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
		return const_cast<reference>(ScriptArrayHelperConstIterator::operator*());
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
		return const_cast<reference>(
		    ScriptArrayHelperConstIterator::operator[](offset));
	}
};
} // namespace udon

int32 UUdonArrayUtilsLibrary::GenericAdjacentFind(
    const void* TargetArray, const FArrayProperty& ArrayProperty,
    UFunction& BinaryPredicate) {
	using namespace udon;

	// helper to allow manipulation of the actual array
	FScriptArrayHelper ArrayHelper(&ArrayProperty, TargetArray);

	// get length of the array
	const auto& NumArray = ArrayHelper.Num();

	// get property of the element
	FProperty* ElemProp = ArrayProperty.Inner;

	// get the size of one element
	const auto& ElemSize = ElemProp->ElementSize;

	// allocate memory for function parameters
	// argument current, next (2 * ElemSize)
	// return value (sizeof(bool))
	void* const PredParam = ::operator new(ElemSize + ElemSize + sizeof(bool));

	// create the begin and end iterators of the TargetArray
	auto begin = ScriptArrayHelperConstIterator(ArrayHelper, ElemSize, 0);
	auto end   = ScriptArrayHelperConstIterator(ArrayHelper, ElemSize, NumArray);

	// Check if all elements of TargetArray satisfy Predicate
	auto found_it = std::adjacent_find(
	    begin, end,
	    [&](const memory_transparent_reference& current,
	        const memory_transparent_reference& next) {
		    // check sizes
		    check(ElemSize == current.mem_size);
		    check(ElemSize == next.mem_size);

		    // get raw pointer of current
		    const auto* const ptr_current = current.target_ptr;

		    // get raw pointer of next
		    const auto* const ptr_next = next.target_ptr;

		    // copy data to PredParam
		    std::memcpy(PredParam, ptr_current, current.mem_size);
		    std::memcpy(static_cast<uint8*>(PredParam) + current.mem_size, ptr_next,
		                next.mem_size);

		    // call BinaryPredicate
		    BinaryPredicate.GetOuter()->ProcessEvent(&BinaryPredicate, PredParam);

		    // get the result of BinaryPredicate call
		    bool BinaryPredicateResult = *reinterpret_cast<bool*>(
		        static_cast<uint8*>(PredParam) + current.mem_size + next.mem_size);

		    // return BinaryPredicateResult
		    return BinaryPredicateResult;
	    });

	// delete memory
	::operator delete(PredParam);

	return std::distance(begin, found_it);
}

bool UUdonArrayUtilsLibrary::GenericAllSatisfy(
    const void* const TargetArray, const FArrayProperty& ArrayProperty,
    UFunction& Predicate) {
	using namespace udon;

	// helper to allow manipulation of the actual array
	FScriptArrayHelper ArrayHelper(&ArrayProperty, TargetArray);

	// get length of the array
	const auto& NumArray = ArrayHelper.Num();

	// get property of the element
	FProperty* ElemProp = ArrayProperty.Inner;

	// get the size of one element
	const auto& ElemSize = ElemProp->ElementSize;

	// allocate memory for function parameters
	// argument ElemSize
	// return value (sizeof(bool))
	void* const PredParam = ::operator new(ElemSize + sizeof(bool));

	// create the begin and end iterators of the TargetArray
	auto begin = ScriptArrayHelperConstIterator(ArrayHelper, ElemSize, 0);
	auto end   = ScriptArrayHelperConstIterator(ArrayHelper, ElemSize, NumArray);

	// Check if all elements of TargetArray satisfy Predicate
	auto bIsAllSatisfy =
	    std::all_of(begin, end, [&](const memory_transparent_reference& elem) {
		    // check sizes
		    check(ElemSize == elem.mem_size);

		    // get raw pointer of elem
		    const auto* const ptr_elem = elem.target_ptr;

		    // copy data to PredParam
		    std::memcpy(PredParam, ptr_elem, elem.mem_size);

		    // call Predicate
		    Predicate.GetOuter()->ProcessEvent(&Predicate, PredParam);

		    // get the result of Predicate call
		    bool PredicateResult = *reinterpret_cast<bool*>(
		        static_cast<uint8*>(PredParam) + elem.mem_size);

		    // return ComparisonResult
		    return PredicateResult;
	    });

	// delete memory
	::operator delete(PredParam);

	return bIsAllSatisfy;
}

bool UUdonArrayUtilsLibrary::GenericAnySatisfy(
    const void* TargetArray, const FArrayProperty& ArrayProperty,
    UFunction& Predicate) {
	using namespace udon;

	// helper to allow manipulation of the actual array
	FScriptArrayHelper ArrayHelper(&ArrayProperty, TargetArray);

	// get length of the array
	const auto& NumArray = ArrayHelper.Num();

	// get property of the element
	FProperty* ElemProp = ArrayProperty.Inner;

	// get the size of one element
	const auto& ElemSize = ElemProp->ElementSize;

	// allocate memory for function parameters
	// argument ElemSize
	// return value (sizeof(bool))
	void* const PredParam = ::operator new(ElemSize + sizeof(bool));

	// create the begin and end iterators of the TargetArray
	auto begin = ScriptArrayHelperConstIterator(ArrayHelper, ElemSize, 0);
	auto end   = ScriptArrayHelperConstIterator(ArrayHelper, ElemSize, NumArray);

	// Check if any element of TargetArray satisfies Predicate
	auto bIsAnySatisfy =
	    std::any_of(begin, end, [&](const memory_transparent_reference& elem) {
		    // check sizes
		    check(ElemSize == elem.mem_size);

		    // get raw pointer of elem
		    const auto* const ptr_elem = elem.target_ptr;

		    // copy data to PredParam
		    std::memcpy(PredParam, ptr_elem, elem.mem_size);

		    // call Predicate
		    Predicate.GetOuter()->ProcessEvent(&Predicate, PredParam);

		    // get the result of Predicate call
		    bool PredicateResult = *reinterpret_cast<bool*>(
		        static_cast<uint8*>(PredParam) + elem.mem_size);

		    // return ComparisonResult
		    return PredicateResult;
	    });

	// delete memory
	::operator delete(PredParam);

	return bIsAnySatisfy;
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
	FProperty* ElemProp = ArrayProperty.Inner;

	// get the size of one element
	const auto& ElemSize = ElemProp->ElementSize;

	// allocate memory for function parameters
	// argument A, B (2 * ElemSize)
	// return value (sizeof(bool))
	void* const ComparisonFunctionParam =
	    ::operator new(ElemSize + ElemSize + sizeof(bool));

	// create the begin and end iterators of the TargetArray
	auto begin = ScriptArrayHelperIterator(ArrayHelper, ElemSize, 0);
	auto end   = ScriptArrayHelperIterator(ArrayHelper, ElemSize, NumArray);

	// sort the elements of TargetArray
	std::sort(
	    begin, end,
	    [&](const memory_transparent_reference& a,
	        const memory_transparent_reference& b) {
		    // check sizes
		    check(ElemSize == a.mem_size);
		    check(ElemSize == b.mem_size);

		    // get raw pointer of a
		    const auto* const ptr_a = a.target_ptr;

		    // get raw pointer of b
		    const auto* const ptr_b = b.target_ptr;

		    // copy data to ComparisonFunctionParam
		    std::memcpy(ComparisonFunctionParam, ptr_a, a.mem_size);
		    std::memcpy(static_cast<uint8*>(ComparisonFunctionParam) + a.mem_size,
		                ptr_b, b.mem_size);

		    // call ComparisonFunction
		    ComparisonFunction.GetOuter()->ProcessEvent(&ComparisonFunction,
		                                                ComparisonFunctionParam);

		    // get the result of ComparisonFunction call
		    bool ComparisonResult = *reinterpret_cast<bool*>(
		        static_cast<uint8*>(ComparisonFunctionParam) + a.mem_size +
		        b.mem_size);

		    // return ComparisonResult
		    return ComparisonResult;
	    });

	// delete memory
	::operator delete(ComparisonFunctionParam);
}
