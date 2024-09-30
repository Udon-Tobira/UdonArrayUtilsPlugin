// Copyright 2024 Udon-Tobira, All Rights Reserved.

#include "BlueprintSortLibrary.h"

/**
 * A class that can be used to swap memory areas for the actual target.
 * Swapping an instance of this class with the Swap function will swap the
 * contents in the actual memory.
 */
class UdonTransparentlySwappablePtr {
public:
	// constructor
	UdonTransparentlySwappablePtr(void* InTargetPtr, int32 InSize)
	    : TargetPtr(InTargetPtr), Size(InSize) {}

	// delete copy, move constructors
	UdonTransparentlySwappablePtr(const UdonTransparentlySwappablePtr&)  = delete;
	UdonTransparentlySwappablePtr(UdonTransparentlySwappablePtr&& other) = delete;

public:
	// delete copy, move assignment operators
	UdonTransparentlySwappablePtr&
	    operator=(const UdonTransparentlySwappablePtr& other) = delete;
	UdonTransparentlySwappablePtr&
	    operator=(UdonTransparentlySwappablePtr&& other) = delete;

public:
	void*       TargetPtr;
	const int32 Size;
};

/**
 * swap instances of UdonTransparentlySwappablePtr
 */
template <>
inline void Swap(UdonTransparentlySwappablePtr& A,
                 UdonTransparentlySwappablePtr& B) {
	// The size of the point destination of A must be as same as the size of the
	// point destination of B.
	check(A.Size == B.Size);

	// swap contents of TargetPtr
	FMemory::Memswap(A.TargetPtr, B.TargetPtr, A.Size);
}

void UUdonBlueprintSortLibrary::GenericSortAnyArray(
    void* TargetArray, const FArrayProperty& ArrayProperty,
    UFunction& ComparisonFunction) {
	// helper to allow manipulation of the actual array
	FScriptArrayHelper ArrayHelper(&ArrayProperty, TargetArray);

	// get length of the array
	const auto& NumArray = ArrayHelper.Num();

	// get property of the element
	FProperty* ElemProp = ArrayProperty.Inner;

	// get the size of one element
	const auto& ElemSize = ElemProp->ElementSize;

	// create array of UdonTransparentlySwappablePtr for TargetArray
	TArray<UdonTransparentlySwappablePtr> TransparentlySorter;
	TransparentlySorter.Reserve(NumArray);
	for (auto i = decltype(NumArray){0}; i < NumArray; ++i) {
		TransparentlySorter.Emplace(ArrayHelper.GetRawPtr(i), ElemSize);
	}

	// sort the elements of TargetArray
	TransparentlySorter.Sort([&](const UdonTransparentlySwappablePtr& A,
	                             const UdonTransparentlySwappablePtr& B) {
		// get raw pointer of A
		const auto* const PtrA = A.TargetPtr;

		// get raw pointer of B
		const auto* const PtrB = B.TargetPtr;

		// allocate memory for function parameters
		// argument A, B (2 * ElemSize)
		// return value (sizeof(bool))
		void* ComparisonFunctionParam = FMemory_Alloca(2 * ElemSize + sizeof(bool));

		// copy data to ComparisonFunctionParam
		FMemory::Memcpy(ComparisonFunctionParam, PtrA, ElemSize);
		FMemory::Memcpy(static_cast<uint8*>(ComparisonFunctionParam) + ElemSize,
		                PtrB, ElemSize);

		// call ComparisonFunction
		ComparisonFunction.GetOuter()->ProcessEvent(&ComparisonFunction,
		                                            ComparisonFunctionParam);

		// get the result of ComparisonFunction call
		bool ComparisonResult = *reinterpret_cast<bool*>(
		    static_cast<uint8*>(ComparisonFunctionParam) + 2 * ElemSize);

		// return ComparisonResult
		return ComparisonResult;
	});
}
