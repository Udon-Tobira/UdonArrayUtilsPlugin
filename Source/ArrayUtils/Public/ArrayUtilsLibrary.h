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
UCLASS()
class ARRAYUTILS_API UUdonArrayUtilsLibrary: public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	/**
	 * Sort an array of any type according to the order of the specified
	 * comparison function.
	 * @param TargetArray  sort target array
	 * @param Object  An object for which the comparison function is defined.
	 * @param ComparisonFunctionName
	 *    The name of a binary predicate function used to specify if one element
	 *    should precede another. This must be a function that has two arguments
	 *    of the same type as the array elements and returns a bool. You should
	 *    return true if the first argument should precede the second.
	 */
	UFUNCTION(BlueprintCallable, Category = "Utilities|Array|Sort", CustomThunk,
	          meta = (CompactNodeTitle = "SORT", DefaultToSelf = "Object",
	                  ArrayParm = "TargetArray"))
	static void SortAnyArray(UPARAM(ref) TArray<int32>& TargetArray,
	                         UObject*                   Object,
	                         const FName&               ComparisonFunctionName);
	// memo: TArray<int32> is actually, TArray<WildCard> type. (because of
	// CustomThunk)

public:
	/**
	 * Sort an array according to the order of the specified comparison function.
	 * @param TargetArray  pointer to sort target array
	 * @param ArrayProperty  property of TargetArray
	 * @param ComparisonFunction
	 *    A binary predicate function used to specify if one element should
	 *    precede another. This must be a function that has two arguments of the
	 *    same type as the array elements and returns a bool. You should return
	 *    true if the first argument should precede the second.
	 */
	static void GenericSortAnyArray(void*                 TargetArray,
	                                const FArrayProperty& ArrayProperty,
	                                UFunction&            ComparisonFunction);

public:
	DECLARE_FUNCTION(execSortAnyArray) {
		///////////////////////////////////
		// read argument 0 (TargetArray) //
		///////////////////////////////////

		// reset MostRecentProperty
		Stack.MostRecentProperty = nullptr;

		// read an array from Stack
		Stack.StepCompiledIn<FArrayProperty>(nullptr);

		// if failed to read an array
		if (!Stack.MostRecentProperty) {
			// notify that failed
			Stack.bArrayContextFailed = true;

			// finish
			return;
		}

		// get pointer to read array
		void* TargetArrayAddr = Stack.MostRecentPropertyAddress;

		// get property of read array
		FArrayProperty* TargetArrayProperty =
		    CastField<FArrayProperty>(Stack.MostRecentProperty);

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
