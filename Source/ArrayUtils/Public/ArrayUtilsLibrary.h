// Copyright 2024 Udon-Tobira, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LogArrayUtilsLibrary.h"
#include "Net/Core/PushModel/PushModel.h"
#include "UObject/UnrealType.h"

#include "ArrayUtilsLibrary.generated.h"

/**
 * Blueprint Function Library of array-related functions.
 */
// memo: In functions where CustomThunk is specified,
// TArray<int32> is actually, TArray<WildCard> type.
UCLASS()
class ARRAYUTILS_API UUdonArrayUtilsLibrary: public UBlueprintFunctionLibrary {
	GENERATED_BODY()

	// <algorithm>
public:
	/**
	 * Searches for the first pair of adjacent elements that satisfy the
	 * condition. Returns the index of the first element of such a pair.
	 * @param TargetArray  target array
	 * @param Object  An object for which the predicate is defined.
	 * @param BinaryPredicateName
	 *    The name of a binary predicate function that defines whether the pair of
	 *    adjacent elements satisfies the condition. This must be a function that
	 *    has two arguments of the same type as the array elements and returns a
	 *    bool. If the pair is considered to meet your intended condition,
	 *    return true; otherwise, return false.
	 * @return
	 *    Returns the index of the first element of the first pair of adjacent
	 *    elements such that the function specified in BinaryPredicateName returns
	 *    true. If not found, return the number of elements in the array
	 *    (means out of index).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utilities|Array",
	          CustomThunk,
	          meta = (CompactNodeTitle = "Adj. Find", DefaultToSelf = "Object",
	                  ArrayParm = "TargetArray",
	                  KeyWords  = "adjacent find predicate"))
	static int32 AdjacentFind(const TArray<int32>& TargetArray, UObject* Object,
	                          const FName& BinaryPredicateName);

	/**
	 * Checks whether all elements of the array satisfy the specified predicate.
	 * @param TargetArray  target array
	 * @param Object  An object for which the predicate is defined.
	 * @param PredicateName
	 *    The name of a unary predicate function that defines whether the element
	 *    satisfies the condition. This must be a function that has one argument
	 *    of the same type as the array elements and returns a bool. If the
	 *    element is considered to meet your intended condition, return true;
	 *    otherwise, return false.
	 * @return
	 *    If the function specified in PredicateName for all elements returns
	 *    true, this function returns true; otherwise, returns false.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utilities|Array",
	          CustomThunk,
	          meta = (CompactNodeTitle = "All", DefaultToSelf = "Object",
	                  ArrayParm = "TargetArray",
	                  KeyWords  = "all of predicate condition"))
	static bool AllSatisfy(const TArray<int32>& TargetArray, UObject* Object,
	                       const FName& PredicateName);

	/**
	 * Checks whether any element of the array satisfies the specified predicate.
	 * @param TargetArray  target array
	 * @param Object  An object for which the predicate is defined.
	 * @param PredicateName
	 *    The name of a unary predicate function that defines whether the element
	 *    satisfies the condition. This must be a function that has one argument
	 *    of the same type as the array elements and returns a bool. If the
	 *    element is considered to meet your intended condition, return true;
	 *    otherwise, return false.
	 * @return
	 *    If the function specified in PredicateName for any element returns
	 *    true, this function returns true; otherwise, returns false.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utilities|Array",
	          CustomThunk,
	          meta = (CompactNodeTitle = "Any", DefaultToSelf = "Object",
	                  ArrayParm = "TargetArray",
	                  KeyWords  = "any of predicate condition"))
	static bool AnySatisfy(const TArray<int32>& TargetArray, UObject* Object,
	                       const FName& PredicateName);

	/**
	 * Sort an array of any type according to the order of the specified
	 * comparison function.
	 * @param TargetArray  sort target array
	 * @param Object  An object for which the comparison function is defined.
	 * @param ComparisonFunctionName
	 *    The name of a comparison function used to specify if one element
	 *    should precede another. This must be a function that has two arguments
	 *    of the same type as the array elements and returns a bool. You should
	 *    return true if the first argument should precede the second; otherwise,
	 *    return false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Utilities|Array|Sort", CustomThunk,
	          meta = (CompactNodeTitle = "SORT", DefaultToSelf = "Object",
	                  ArrayParm = "TargetArray", KeyWords = "sort order arrange"))
	static void SortAnyArray(UPARAM(ref) TArray<int32>& TargetArray,
	                         UObject*                   Object,
	                         const FName&               ComparisonFunctionName);

public:
	static int32 GenericAdjacentFind(const void*           TargetArray,
	                                 const FArrayProperty& ArrayProperty,
	                                 UFunction&            BinaryPredicate);

	/**
	 * Checks whether all elements of the array satisfy the specified predicate.
	 * @param TargetArray  target array
	 * @param ArrayProperty  property of TargetArray
	 * @param Predicate
	 *    A unary predicate function that defines whether the element
	 *    satisfies the condition. This must be a function that has one argument
	 *    of the same type as the array elements and returns a bool. If the
	 *    element is considered to meet your intended condition, return true;
	 *    otherwise, return false.
	 * @return
	 *    If Predicate for all elements returns true, this function returns true;
	 *    otherwise, returns false.
	 */
	static bool GenericAllSatisfy(const void*           TargetArray,
	                              const FArrayProperty& ArrayProperty,
	                              UFunction&            Predicate);

	/**
	 * Checks whether any element of the array satisfies the specified predicate.
	 * @param TargetArray  target array
	 * @param ArrayProperty  property of TargetArray
	 * @param Predicate
	 *    A unary predicate function that defines whether the element
	 *    satisfies the condition. This must be a function that has one argument
	 *    of the same type as the array elements and returns a bool. If the
	 *    element is considered to meet your intended condition, return true;
	 *    otherwise, return false.
	 * @return
	 *    If Predicate for any element returns true, this function returns true;
	 *    otherwise, returns false.
	 */
	static bool GenericAnySatisfy(const void*           TargetArray,
	                              const FArrayProperty& ArrayProperty,
	                              UFunction&            Predicate);

	/**
	 * Sort an array according to the order of the specified comparison function.
	 * @param TargetArray  pointer to sort target array
	 * @param ArrayProperty  property of TargetArray
	 * @param ComparisonFunction
	 *    A comparison function used to specify if one element should
	 *    precede another. This must be a function that has two arguments of the
	 *    same type as the array elements and returns a bool. You should return
	 *    true if the first argument should precede the second; otherwise,
	 *    return false.
	 */
	static void GenericSortAnyArray(void*                 TargetArray,
	                                const FArrayProperty& ArrayProperty,
	                                UFunction&            ComparisonFunction);

public:
	DECLARE_FUNCTION(execAdjacentFind) {
		///////////////////////////////////
		// read argument 0 (TargetArray) //
		///////////////////////////////////

		// reset MostRecentProperty
		Stack.MostRecentProperty = nullptr;

		// read an array from Stack
		Stack.StepCompiledIn<FArrayProperty>(nullptr);

		// get pointer to read array
		const void* TargetArrayAddr = Stack.MostRecentPropertyAddress;

		// get property of read array
		FArrayProperty* TargetArrayProperty =
		    CastField<FArrayProperty>(Stack.MostRecentProperty);

		// if failed to read an array
		if (!TargetArrayProperty) {
			// notify that failed
			Stack.bArrayContextFailed = true;

			// finish
			return;
		}

		//////////////////////////////
		// read argument 1 (Object) //
		//////////////////////////////
		P_GET_PROPERTY(FObjectProperty, Object);

		//////////////////////////////////////////////
		// read argument 2 (BinaryPredicateName) //
		//////////////////////////////////////////////
		P_GET_PROPERTY(FNameProperty, BinaryPredicateName);

		// end of reading arguments
		P_FINISH;

		// beginning of native processing
		P_NATIVE_BEGIN;

		// get BinaryPredicate on Object
		const auto& BinaryPredicate = Object->FindFunction(BinaryPredicateName);

		// if binary predicate doesn't exist
		if (!BinaryPredicate) {
			// output error
			UE_LOG(LogUdonArrayUtilsLibrary, Error,
			       TEXT("Binary predicate '%s' not found on object: %s"),
			       *BinaryPredicateName.ToString(), *Object->GetName());

			// finish
			return;
		}

		// Perform the adjacent find
		*static_cast<int32*>(RESULT_PARAM) = GenericAdjacentFind(
		    TargetArrayAddr, *TargetArrayProperty, *BinaryPredicate);

		// end of native processing
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execAllSatisfy) {
		///////////////////////////////////
		// read argument 0 (TargetArray) //
		///////////////////////////////////

		// reset MostRecentProperty
		Stack.MostRecentProperty = nullptr;

		// read an array from Stack
		Stack.StepCompiledIn<FArrayProperty>(nullptr);

		// get pointer to read array
		const void* TargetArrayAddr = Stack.MostRecentPropertyAddress;

		// get property of read array
		FArrayProperty* TargetArrayProperty =
		    CastField<FArrayProperty>(Stack.MostRecentProperty);

		// if failed to read an array
		if (!TargetArrayProperty) {
			// notify that failed
			Stack.bArrayContextFailed = true;

			// finish
			return;
		}

		//////////////////////////////
		// read argument 1 (Object) //
		//////////////////////////////
		P_GET_PROPERTY(FObjectProperty, Object);

		//////////////////////////////////////////////
		// read argument 2 (PredicateName) //
		//////////////////////////////////////////////
		P_GET_PROPERTY(FNameProperty, PredicateName);

		// end of reading arguments
		P_FINISH;

		// beginning of native processing
		P_NATIVE_BEGIN;

		// get Predicate on Object
		const auto& Predicate = Object->FindFunction(PredicateName);

		// if predicate doesn't exist
		if (!Predicate) {
			// output error
			UE_LOG(LogUdonArrayUtilsLibrary, Error,
			       TEXT("Predicate '%s' not found on object: %s"),
			       *PredicateName.ToString(), *Object->GetName());

			// finish
			return;
		}

		// Perform the all_of
		*static_cast<bool*>(RESULT_PARAM) =
		    GenericAllSatisfy(TargetArrayAddr, *TargetArrayProperty, *Predicate);

		// end of native processing
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execAnySatisfy) {
		///////////////////////////////////
		// read argument 0 (TargetArray) //
		///////////////////////////////////

		// reset MostRecentProperty
		Stack.MostRecentProperty = nullptr;

		// read an array from Stack
		Stack.StepCompiledIn<FArrayProperty>(nullptr);

		// get pointer to read array
		const void* TargetArrayAddr = Stack.MostRecentPropertyAddress;

		// get property of read array
		FArrayProperty* TargetArrayProperty =
		    CastField<FArrayProperty>(Stack.MostRecentProperty);

		// if failed to read an array
		if (!TargetArrayProperty) {
			// notify that failed
			Stack.bArrayContextFailed = true;

			// finish
			return;
		}

		//////////////////////////////
		// read argument 1 (Object) //
		//////////////////////////////
		P_GET_PROPERTY(FObjectProperty, Object);

		//////////////////////////////////////////////
		// read argument 2 (PredicateName) //
		//////////////////////////////////////////////
		P_GET_PROPERTY(FNameProperty, PredicateName);

		// end of reading arguments
		P_FINISH;

		// beginning of native processing
		P_NATIVE_BEGIN;

		// get Predicate on Object
		const auto& Predicate = Object->FindFunction(PredicateName);

		// if predicate doesn't exist
		if (!Predicate) {
			// output error
			UE_LOG(LogUdonArrayUtilsLibrary, Error,
			       TEXT("Predicate '%s' not found on object: %s"),
			       *PredicateName.ToString(), *Object->GetName());

			// finish
			return;
		}

		// Perform the any_of
		*static_cast<bool*>(RESULT_PARAM) =
		    GenericAnySatisfy(TargetArrayAddr, *TargetArrayProperty, *Predicate);

		// end of native processing
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execSortAnyArray) {
		///////////////////////////////////
		// read argument 0 (TargetArray) //
		///////////////////////////////////

		// reset MostRecentProperty
		Stack.MostRecentProperty = nullptr;

		// read an array from Stack
		Stack.StepCompiledIn<FArrayProperty>(nullptr);

		// get pointer to read array
		void* TargetArrayAddr = Stack.MostRecentPropertyAddress;

		// get property of read array
		FArrayProperty* TargetArrayProperty =
		    CastField<FArrayProperty>(Stack.MostRecentProperty);

		// if failed to read an array
		if (!TargetArrayProperty) {
			// notify that failed
			Stack.bArrayContextFailed = true;

			// finish
			return;
		}

		//////////////////////////////
		// read argument 1 (Object) //
		//////////////////////////////
		P_GET_PROPERTY(FObjectProperty, Object);

		//////////////////////////////////////////////
		// read argument 2 (ComparisonFunctionName) //
		//////////////////////////////////////////////
		P_GET_PROPERTY(FNameProperty, ComparisonFunctionName);

		// end of reading arguments
		P_FINISH;

		// beginning of native processing
		P_NATIVE_BEGIN;

		// get ComparisonFunction on Object
		const auto& ComparisonFunction =
		    Object->FindFunction(ComparisonFunctionName);

		// if comparison function doesn't exist
		if (!ComparisonFunction) {
			// output error
			UE_LOG(LogUdonArrayUtilsLibrary, Error,
			       TEXT("Comparison function '%s' not found on object: %s"),
			       *ComparisonFunctionName.ToString(), *Object->GetName());

			// finish
			return;
		}

		// Perform the sort
		MARK_PROPERTY_DIRTY(Stack.Object, TargetArrayProperty);
		GenericSortAnyArray(TargetArrayAddr, *TargetArrayProperty,
		                    *ComparisonFunction);

		// end of native processing
		P_NATIVE_END;
	}
};
