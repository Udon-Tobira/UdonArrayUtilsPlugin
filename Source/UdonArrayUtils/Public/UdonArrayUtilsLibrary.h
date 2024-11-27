// Copyright 2024 Udon-Tobira, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LogUdonArrayUtilsLibrary.h"
#include "Net/Core/PushModel/PushModel.h"
#include "UObject/UnrealType.h"

#include <memory>

#include "UdonArrayUtilsLibrary.generated.h"

/**
 * Blueprint Function Library of array-related functions.
 */
// memo: In functions where CustomThunk is specified,
// TArray<int32> is actually, TArray<WildCard> type.
UCLASS()
class UDONARRAYUTILS_API UUdonArrayUtilsLibrary
    : public UBlueprintFunctionLibrary {
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
	 *    true. If not found, returns INDEX_NONE (means out of index).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utilities|Array",
	          CustomThunk,
	          meta = (CompactNodeTitle = "Adj. FIND", DefaultToSelf = "Object",
	                  ArrayParm         = "TargetArray",
	                  AutoCreateRefTerm = "BinaryPredicateName",
	                  KeyWords          = "adjacent find predicate"))
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
	          meta = (CompactNodeTitle = "ALL", DefaultToSelf = "Object",
	                  ArrayParm         = "TargetArray",
	                  AutoCreateRefTerm = "PredicateName",
	                  KeyWords          = "all of predicate condition"))
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
	          meta = (CompactNodeTitle = "ANY", DefaultToSelf = "Object",
	                  ArrayParm         = "TargetArray",
	                  AutoCreateRefTerm = "PredicateName",
	                  KeyWords          = "any of predicate condition"))
	static bool AnySatisfy(const TArray<int32>& TargetArray, UObject* Object,
	                       const FName& PredicateName);

	/**
	 * Count the number of elements that match the specified element.
	 * @param TargetArray  target array
	 * @param ItemToCount  An element to be counted
	 * @return  The number of elements that match ItemToCount.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utilities|Array",
	          CustomThunk,
	          meta = (CompactNodeTitle = "COUNT", ArrayParm = "TargetArray",
	                  ArrayTypeDependentParams = "ItemToCount",
	                  AutoCreateRefTerm        = "ItemToCount"))
	static int32 Count(const TArray<int32>& TargetArray,
	                   const int32&         ItemToCount);

	/**
	 * Count the number of elements that satisfy the condition.
	 * @param TargetArray  target array
	 * @param Object   An object for which the predicate is defined.
	 * @param PredicateName
	 *    The name of a unary predicate function that defines whether the element
	 *    satisfies the condition. This must be a function that has one argument
	 *    of the same type as the array elements and returns a bool. If the
	 *    element is considered to meet your intended condition, return true;
	 *    otherwise, return false.
	 * @return
	 *    The total number of elements that returned true when the function with
	 *    the name specified in PredicateName was applied to each elements.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utilities|Array",
	          CustomThunk,
	          meta = (CompactNodeTitle = "COUNT IF", DefaultToSelf = "Object",
	                  ArrayParm         = "TargetArray",
	                  AutoCreateRefTerm = "PredicateName",
	                  KeyWords          = "count if predicate condition"))
	static int32 CountIf(const TArray<int32>& TargetArray, UObject* Object,
	                     const FName& PredicateName);

	/**
	 * Overwrites the entire array with Value.
	 * @param TargetArray  target array
	 * @param Value  the value to be written to the entire array
	 */
	UFUNCTION(BlueprintCallable, Category = "Utilities|Array", CustomThunk,
	          meta = (CompactNodeTitle = "FILL", ArrayParm = "TargetArray",
	                  ArrayTypeDependentParams = "Value",
	                  AutoCreateRefTerm = "Value", KeyWords = "fill set all"))
	static void Fill(UPARAM(ref) TArray<int32>& TargetArray, const int32& Value);

	/**
	 * Overwrites the range [StartIndex, EndIndex) of the array with Value.
	 * @param TargetArray  target array
	 * @param StartIndex  the index of the first element to be overwritten.
	 * @param EndIndex  the index of the next element after the last overwritten
	 *                  element
	 * @param Value  the value to be written to the range
	 */
	UFUNCTION(BlueprintCallable, Category = "Utilities|Array", CustomThunk,
	          meta = (ArrayParm                = "TargetArray",
	                  ArrayTypeDependentParams = "Value",
	                  AutoCreateRefTerm = "Value", KeyWords = "fill set range"))
	static void FillRange(UPARAM(ref) TArray<int32>& TargetArray,
	                      int32 StartIndex, int32 EndIndex, const int32& Value);

	/**
	 * Searches for the first element that satisfies the specified predicate.
	 * @param TargetArray  target array
	 * @param Object  An object for which the predicate is defined.
	 * @param PredicateName
	 *    The name of a unary predicate function that defines whether the element
	 *    satisfies the condition. This must be a function that has one argument
	 *    of the same type as the array elements and returns a bool. If the
	 *    element is considered to meet your intended condition, return true;
	 *    otherwise, return false.
	 * @return
	 *    Returns the index of the first element that satisfies the predicate.
	 *    If not found, returns INDEX_NONE (means out of index).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utilities|Array",
	          CustomThunk,
	          meta = (CompactNodeTitle = "FIND IF", DefaultToSelf = "Object",
	                  ArrayParm         = "TargetArray",
	                  AutoCreateRefTerm = "PredicateName",
	                  KeyWords          = "find if predicate condition"))
	static int32 FindIf(const TArray<int32>& TargetArray, UObject* Object,
	                    const FName& PredicateName);

	/**
	 * Finds the maximum element in the array using a comparison function.
	 * @param TargetArray  target array
	 * @param Object  An object for which the comparison function is defined.
	 * @param ComparisonFunctionName
	 *    The Name of a comparison function that determines which of two elements
	 *    is greater. You should return true if the first argument is less than
	 *    the second; otherwise, return false.
	 * @param[out] MaxValue
	 *    The maximum element in the array. If the array is empty, this lvalue is
	 *    not modified.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utilities|Array",
	          CustomThunk,
	          meta = (CompactNodeTitle = "MAX", DefaultToSelf = "Object",
	                  ArrayParm                = "TargetArray",
	                  ArrayTypeDependentParams = "MaxValue",
	                  AutoCreateRefTerm        = "ComparisonFunctionName",
	                  KeyWords = "max maximum predicate compare comparison"))
	static void Max(const TArray<int32>& TargetArray, UObject* Object,
	                const FName&   ComparisonFunctionName,
	                /*out*/ int32& MaxValue);

	/**
	 * Searches for the index of the maximum element in the array using a custom
	 * comparison function.
	 * @param TargetArray  The target array to search.
	 * @param Object  An object for which the comparison function is defined.
	 * @param ComparisonFunctionName
	 *    The name of a binary comparison function
	 *    that defines the order of elements. This function must have two
	 *    arguments of the same type as the array elements and return a bool. If
	 *    the first element is considered to be less than the second element,
	 *    return true; otherwise, return false.
	 * @return
	 *    The index of the maximum element in the array. If the array is
	 *    empty, returns INDEX_NONE.
	 */
	UFUNCTION(
	    BlueprintCallable, BlueprintPure, Category = "Utilities|Array",
	    CustomThunk,
	    meta =
	        (CompactNodeTitle = "MAX Elem. INDEX", DefaultToSelf = "Object",
	         ArrayParm         = "TargetArray",
	         AutoCreateRefTerm = "ComparisonFunctionName",
	         KeyWords =
	             "max maximum elem element index predicate compare comparison"))
	static int32 MaxElementIndex(const TArray<int32>& TargetArray,
	                             UObject*             Object,
	                             const FName&         ComparisonFunctionName);

	/**
	 * Finds the minimum element in the array using a comparison function.
	 * @param TargetArray  target array
	 * @param Object  An object for which the comparison function is defined.
	 * @param ComparisonFunctionName
	 *    The Name of a comparison function that determines which of two elements
	 *    is greater. You should return true if the first argument is less than
	 *    the second; otherwise, return false.
	 * @param[out] MinValue
	 *    The minimum element in the array. If the array is empty, this lvalue is
	 *    not modified.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utilities|Array",
	          CustomThunk,
	          meta = (CompactNodeTitle = "MIN", DefaultToSelf = "Object",
	                  ArrayParm                = "TargetArray",
	                  ArrayTypeDependentParams = "MinValue",
	                  AutoCreateRefTerm        = "ComparisonFunctionName",
	                  KeyWords = "min minimum predicate compare comparison"))
	static void Min(const TArray<int32>& TargetArray, UObject* Object,
	                const FName&   ComparisonFunctionName,
	                /*out*/ int32& MinValue);

	/**
	 * Searches for the index of the minimum element in the array using a custom
	 * comparison function.
	 * @param TargetArray  The target array to search.
	 * @param Object  An object for which the comparison function is defined.
	 * @param ComparisonFunctionName
	 *    The name of a binary comparison function
	 *    that defines the order of elements. This function must have two
	 *    arguments of the same type as the array elements and return a bool. If
	 *    the first element is considered to be less than the second element,
	 *    return true; otherwise, return false.
	 * @return
	 *    The index of the minimum element in the array. If the array is
	 *    empty, returns INDEX_NONE.
	 */
	UFUNCTION(
	    BlueprintCallable, BlueprintPure, Category = "Utilities|Array",
	    CustomThunk,
	    meta =
	        (CompactNodeTitle = "MIN Elem. INDEX", DefaultToSelf = "Object",
	         ArrayParm         = "TargetArray",
	         AutoCreateRefTerm = "ComparisonFunctionName",
	         KeyWords =
	             "min minimum elem element index predicate compare comparison"))
	static int32 MinElementIndex(const TArray<int32>& TargetArray,
	                             UObject*             Object,
	                             const FName&         ComparisonFunctionName);

	/**
	 * Checks whether none elements of the array satisfy the specified predicate.
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
	 *    true, this function returns false; otherwise, returns true.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utilities|Array",
	          CustomThunk,
	          meta = (CompactNodeTitle = "NONE", DefaultToSelf = "Object",
	                  ArrayParm         = "TargetArray",
	                  AutoCreateRefTerm = "PredicateName",
	                  KeyWords          = "none of predicate condition"))
	static bool NoneSatisfy(const TArray<int32>& TargetArray, UObject* Object,
	                        const FName& PredicateName);

	/**
	 * Removes the elements in the range [StartIndex, EndIndex) from the target
	 * array.
	 * @param TargetArray  target array
	 * @param StartIndex   the index of the first element to remove
	 * @param EndIndex     the next index of the last element to remove
	 */
	UFUNCTION(BlueprintCallable, Category = "Utilities|Array", CustomThunk,
	          meta = (ArrayParm = "TargetArray",
	                  KeyWords  = "remove delete erase range between"))
	static void RemoveRange(UPARAM(ref) TArray<int32>& TargetArray,
	                        int32 StartIndex, int32 EndIndex);

	/**
	 * Removes elements from the array that satisfy the specified predicate.
	 * @param TargetArray  target array
	 * @param Object  An object for which the predicate is defined.
	 * @param PredicateName
	 *    The name of a unary predicate function that defines whether the element
	 *    satisfies the condition. This must be a function that has one argument
	 *    of the same type as the array elements and returns a bool. If the
	 *    element is considered to meet your intended condition, return true;
	 *    otherwise, return false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Utilities|Array", CustomThunk,
	          meta = (CompactNodeTitle = "REMOVE IF", DefaultToSelf = "Object",
	                  ArrayParm         = "TargetArray",
	                  AutoCreateRefTerm = "PredicateName",
	                  KeyWords = "remove delete erase if predicate condition"))
	static void RemoveIf(UPARAM(ref) TArray<int32>& TargetArray, UObject* Object,
	                     const FName& PredicateName);

	/**
	 * Randomly select the specified number of samples from the target array.
	 * @param TargetArray  target array
	 * @param NumOfSamples  number of samples to randomly select
	 * @param[out] Samples  output array to store the randomly selected samples
	 * @param[out] Others output array to store the remaining elements
	 */
	UFUNCTION(BlueprintCallable, Category = "Utilities|Array", CustomThunk,
	          meta = (AdvancedDisplay          = "Others",
	                  ArrayParm                = "TargetArray,Samples,Others",
	                  ArrayTypeDependentParams = "TargetArray,Samples,Others",
	                  NumOfSamples = 1, KeyWords = "random sample items"))
	static void RandomSample(const TArray<int32>& TargetArray, int32 NumOfSamples,
	                         TArray<int32>& Samples, TArray<int32>& Others);

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
	UFUNCTION(
	    BlueprintCallable, Category = "Utilities|Array|Sort", CustomThunk,
	    meta = (CompactNodeTitle = "SORT", DefaultToSelf = "Object",
	            ArrayParm         = "TargetArray",
	            AutoCreateRefTerm = "ComparisonFunctionName",
	            KeyWords = "sort order arrange predicate compare comparison"))
	static void SortAnyArray(UPARAM(ref) TArray<int32>& TargetArray,
	                         UObject*                   Object,
	                         const FName&               ComparisonFunctionName);

public:
	/**
	 * Searches for the first pair of adjacent elements that satisfy the
	 * condition. Returns the index of the first element of such a pair.
	 * @param TargetArray  target array
	 * @param ArrayProperty  property of TargetArray
	 * @param Object  An object for which the binary predicate function is
	 *                defined.
	 * @param BinaryPredicate
	 *    A binary predicate function that defines whether the pair of
	 *    adjacent elements satisfies the condition. This must be a function that
	 *    has two arguments of the same type as the array elements and returns a
	 *    bool. If the pair is considered to meet your intended condition,
	 *    return true; otherwise, return false.
	 * @return
	 *    Returns the index of the first element of the first pair of adjacent
	 *    elements such that BinaryPredicate returns true. If not found, returns
	 *    INDEX_NONE (means out of index).
	 */
	static int32 GenericAdjacentFind(const void*           TargetArray,
	                                 const FArrayProperty& ArrayProperty,
	                                 UObject& Object, UFunction& BinaryPredicate);

	/**
	 * Checks whether all elements of the array satisfy the specified predicate.
	 * @param TargetArray  target array
	 * @param ArrayProperty  property of TargetArray
	 * @param Object  An object for which the unary predicate function is
	 *                defined.
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
	                              UObject& Object, UFunction& Predicate);

	/**
	 * Checks whether any element of the array satisfies the specified predicate.
	 * @param TargetArray  target array
	 * @param ArrayProperty  property of TargetArray
	 * @param Object  An object for which the unary predicate function is
	 *                defined.
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
	                              UObject& Object, UFunction& Predicate);

	/**
	 * Count the number of elements that match the specified element.
	 * @param TargetArray  target array
	 * @param ArrayProperty  property of TargetArray
	 * @param ItemToCount  An element to be counted
	 * @return  The number of elements that match ItemToCount.
	 */
	static int32 GenericCount(const void*           TargetArray,
	                          const FArrayProperty& ArrayProperty,
	                          const void*           ItemToCount);

	/**
	 * Count the number of elements that satisfy the condition.
	 * @param TargetArray  target array
	 * @param ArrayProperty  property of TargetArray
	 * @param Object  An object for which the unary predicate function is
	 *                defined.
	 * @param Predicate
	 *    A unary predicate function that defines whether the element
	 *    satisfies the condition. This must be a function that has one argument
	 *    of the same type as the array elements and returns a bool. If the
	 *    element is considered to meet your intended condition, return true;
	 *    otherwise, return false.
	 * @return
	 *    The total number of elements that returned true when the function with
	 *    the name specified in PredicateName was applied to each elements.
	 */
	static int32 GenericCountIf(const void*           TargetArray,
	                            const FArrayProperty& ArrayProperty,
	                            UObject& Object, UFunction& Predicate);

	/**
	 * Overwrites the entire array with Value.
	 * @param TargetArray  target array
	 * @param ArrayProperty  property of TargetArray
	 * @param Value  the value to be written to the entire array
	 */
	static void GenericFill(void*                 TargetArray,
	                        const FArrayProperty& ArrayProperty,
	                        const void*           Value);

	/**
	 * Overwrites the range [StartIndex, EndIndex) of the array with Value.
	 * @param TargetArray  target array
	 * @param ArrayProperty  property of TargetArray
	 * @param StartIndex  the index of the first element to be overwritten.
	 * @param EndIndex  the index of the next element after the last overwritten
	 *                  element
	 * @param Value  the value to be written to the range
	 */
	static void GenericFill(void*                 TargetArray,
	                        const FArrayProperty& ArrayProperty, int32 StartIndex,
	                        int32 EndIndex, const void* Value);

	/**
	 * Searches for the first element that satisfies the specified predicate.
	 * @param TargetArray  target array
	 * @param ArrayProperty  property of TargetArray
	 * @param Object  An object for which the unary predicate function is
	 *                defined.
	 * @param Predicate
	 *    A unary predicate function that defines whether the element
	 *    satisfies the condition. This must be a function that has one argument
	 *    of the same type as the array elements and returns a bool. If the
	 *    element is considered to meet your intended condition, return true;
	 *    otherwise, return false.
	 * @return
	 *    Returns the index of the first element that satisfies the predicate.
	 *    If not found, returns INDEX_NONE (means out of index).
	 */
	static int32 GenericFindIf(const void*           TargetArray,
	                           const FArrayProperty& ArrayProperty,
	                           UObject& Object, UFunction& Predicate);

	/**
	 * Finds the maximum element in the array using a comparison function.
	 * @param TargetArray  target array
	 * @param ArrayProperty  property of TargetArray
	 * @param Object  An object for which the comparison function is defined.
	 * @param ComparisonFunction
	 *    A comparison function that determines which of two elements is greater.
	 *    You should return true if the first argument is less than the second;
	 *    otherwise, return false.
	 * @return  The pointer to the maximum element in the array. If the array is
	 *          empty, returns nullptr.
	 */
	static const void* GenericMax(const void*           TargetArray,
	                              const FArrayProperty& ArrayProperty,
	                              UObject& Object, UFunction& ComparisonFunction);

	/**
	 * Searches for the index of the maximum element in the array using a custom
	 * comparison function.
	 * @param TargetArray  The target array to search.
	 * @param ArrayProperty  property of TargetArray
	 * @param Object  An object for which the binary comparison function is
	 *                defined.
	 * @param ComparisonFunction
	 *    A binary comparison function that defines the order of elements. This
	 *    function must have two arguments of the same type as the array elements
	 *    and return a bool. If the first element is considered to be less than
	 *    the second element, return true; otherwise, return false.
	 * @return
	 *    The index of the maximum element in the array. If the array is
	 *    empty, returns INDEX_NONE.
	 */
	static int32 GenericMaxElementIndex(const void*           TargetArray,
	                                    const FArrayProperty& ArrayProperty,
	                                    UObject&              Object,
	                                    UFunction&            ComparisonFunction);

	/**
	 * Finds the minimum element in the array using a comparison function.
	 * @param TargetArray  target array
	 * @param ArrayProperty  property of TargetArray
	 * @param Object  An object for which the binary comparison function is
	 *                defined.
	 * @param ComparisonFunction
	 *    A comparison function that determines which of two elements is greater.
	 *    You should return true if the first argument is less than the second;
	 *    otherwise, return false.
	 * @return  The pointer to the minimum element in the array. If the array is
	 *          empty, returns nullptr.
	 */
	static const void* GenericMin(const void*           TargetArray,
	                              const FArrayProperty& ArrayProperty,
	                              UObject& Object, UFunction& ComparisonFunction);

	/**
	 * Searches for the index of the minimum element in the array using a custom
	 * comparison function.
	 * @param TargetArray  The target array to search.
	 * @param ArrayProperty  property of TargetArray
	 * @param Object  An object for which the binary comparison function is
	 *                defined.
	 * @param ComparisonFunction
	 *    A binary comparison function that defines the order of elements. This
	 *    function must have two arguments of the same type as the array elements
	 *    and return a bool. If the first element is considered to be less than
	 *    the second element, return true; otherwise, return false.
	 * @return
	 *    The index of the minimum element in the array. If the array is
	 *    empty, returns INDEX_NONE.
	 */
	static int32 GenericMinElementIndex(const void*           TargetArray,
	                                    const FArrayProperty& ArrayProperty,
	                                    UObject&              Object,
	                                    UFunction&            ComparisonFunction);

	/**
	 * Checks whether none elements of the array satisfy the specified predicate.
	 * @param TargetArray  target array
	 * @param ArrayProperty  property of TargetArray
	 * @param Object  An object for which the unary predicate function is
	 *                defined.
	 * @param Predicate
	 *    A unary predicate function that defines whether the element
	 *    satisfies the condition. This must be a function that has one argument
	 *    of the same type as the array elements and returns a bool. If the
	 *    element is considered to meet your intended condition, return true;
	 *    otherwise, return false.
	 * @return
	 *    If Predicate for any element returns true, this function returns false;
	 *    otherwise, returns true.
	 */
	static bool GenericNoneSatisfy(const void*           TargetArray,
	                               const FArrayProperty& ArrayProperty,
	                               UObject& Object, UFunction& Predicate);

	/**
	 * Removes the elements in the range [StartIndex, EndIndex) from the target
	 * array.
	 * @param TargetArray  target array
	 * @param ArrayProperty  property of TargetArray
	 * @param StartIndex   the index of the first element to remove
	 * @param EndIndex     the next index of the last element to remove
	 */
	static void GenericRemoveRange(void*                 TargetArray,
	                               const FArrayProperty& ArrayProperty,
	                               int32 StartIndex, int32 EndIndex);

	/**
	 * Removes elements from the array that satisfy the specified predicate.
	 * @param TargetArray  target array
	 * @param ArrayProperty  property of TargetArray
	 * @param Object  An object for which the unary predicate function is
	 *                defined.
	 * @param Predicate
	 *    A unary predicate function that defines whether the element
	 *    satisfies the condition. This must be a function that has one argument
	 *    of the same type as the array elements and returns a bool. If the
	 *    element is considered to meet your intended condition, return true;
	 *    otherwise, return false.
	 */
	static void GenericRemoveIf(void*                 TargetArray,
	                            const FArrayProperty& ArrayProperty,
	                            UObject& Object, UFunction& Predicate);

	/**
	 * Randomly select the specified number of samples from the target array.
	 * @param TargetArray  target array
	 * @param ArrayProperty  property of TargetArray
	 * @param NumOfSamples  number of samples to randomly select
	 * @return
	 *    A pair of two arrays. The first array contains the randomly
	 *    selected samples, and the second array contains the remaining elements.
	 */
	static std::pair<std::shared_ptr<FScriptArray>, std::shared_ptr<FScriptArray>>
	    GenericRandomSample(const void*           TargetArray,
	                        const FArrayProperty& ArrayProperty,
	                        int32                 NumOfSamples);

	/**
	 * Sort an array according to the order of the specified comparison function.
	 * @param TargetArray  pointer to sort target array
	 * @param ArrayProperty  property of TargetArray
	 * @param Object  An object for which the binary comparison function is
	 *                defined.
	 * @param ComparisonFunction
	 *    A comparison function used to specify if one element should
	 *    precede another. This must be a function that has two arguments of the
	 *    same type as the array elements and returns a bool. You should return
	 *    true if the first argument should precede the second; otherwise,
	 *    return false.
	 */
	static void GenericSortAnyArray(void*                 TargetArray,
	                                const FArrayProperty& ArrayProperty,
	                                UObject&              Object,
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
		    TargetArrayAddr, *TargetArrayProperty, *Object, *BinaryPredicate);

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
		*static_cast<bool*>(RESULT_PARAM) = GenericAllSatisfy(
		    TargetArrayAddr, *TargetArrayProperty, *Object, *Predicate);

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
		*static_cast<bool*>(RESULT_PARAM) = GenericAnySatisfy(
		    TargetArrayAddr, *TargetArrayProperty, *Object, *Predicate);

		// end of native processing
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execCount) {
		///////////////////////////////////
		// read argument 0 (TargetArray) //
		///////////////////////////////////

		// reset MostRecentProperty
		Stack.MostRecentProperty = nullptr;

		// read an array from Stack
		Stack.StepCompiledIn<FArrayProperty>(nullptr);

		// get pointer to read array
		const void* const TargetArrayAddr = Stack.MostRecentPropertyAddress;

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

		///////////////////////////////////
		// read argument 1 (ItemToCount) //
		///////////////////////////////////
		// Since ItemToCount isn't really an int, step the stack manually

		// reset MostRecentPropertyAddress
		Stack.MostRecentPropertyAddress = nullptr;

		// read a value from Stack
		Stack.StepCompiledIn<FProperty>(nullptr);

		// get pointer to read value
		const auto* const ItemToFindPtr = Stack.MostRecentPropertyAddress;

		// end of reading arguments
		P_FINISH;

		// beginning of native processing
		P_NATIVE_BEGIN;

		// Perform the count
		*static_cast<int32*>(RESULT_PARAM) =
		    GenericCount(TargetArrayAddr, *TargetArrayProperty, ItemToFindPtr);

		// end of native processing
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execCountIf) {
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

		// Perform the count_if
		*static_cast<int32*>(RESULT_PARAM) = GenericCountIf(
		    TargetArrayAddr, *TargetArrayProperty, *Object, *Predicate);

		// end of native processing
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execFill) {
		///////////////////////////////////
		// read argument 0 (TargetArray) //
		///////////////////////////////////

		// reset MostRecentProperty
		Stack.MostRecentProperty = nullptr;

		// read an array from Stack
		Stack.StepCompiledIn<FArrayProperty>(nullptr);

		// get pointer to read array
		void* const TargetArrayAddr = Stack.MostRecentPropertyAddress;

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

		/////////////////////////////
		// read argument 1 (Value) //
		/////////////////////////////
		// Since Value isn't really an int, step the stack manually

		// reset MostRecentPropertyAddress
		Stack.MostRecentPropertyAddress = nullptr;

		// read a value from Stack
		Stack.StepCompiledIn<FProperty>(nullptr);

		// get pointer to read value
		const auto* const Value = Stack.MostRecentPropertyAddress;

		// end of reading arguments
		P_FINISH;

		// beginning of native processing
		P_NATIVE_BEGIN;

		// Perform fill
		MARK_PROPERTY_DIRTY(Stack.Object, TargetArrayProperty);
		GenericFill(TargetArrayAddr, *TargetArrayProperty, Value);

		// end of native processing
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execFillRange) {
		///////////////////////////////////
		// read argument 0 (TargetArray) //
		///////////////////////////////////

		// reset MostRecentProperty
		Stack.MostRecentProperty = nullptr;

		// read an array from Stack
		Stack.StepCompiledIn<FArrayProperty>(nullptr);

		// get pointer to read array
		void* const TargetArrayAddr = Stack.MostRecentPropertyAddress;

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

		//////////////////////////////////
		// read argument 1 (StartIndex) //
		//////////////////////////////////
		P_GET_PROPERTY(FIntProperty, StartIndex);

		////////////////////////////////
		// read argument 2 (EndIndex) //
		////////////////////////////////
		P_GET_PROPERTY(FIntProperty, EndIndex);

		/////////////////////////////
		// read argument 3 (Value) //
		/////////////////////////////
		// Since Value isn't really an int, step the stack manually

		// reset MostRecentPropertyAddress
		Stack.MostRecentPropertyAddress = nullptr;

		// read a value from Stack
		Stack.StepCompiledIn<FProperty>(nullptr);

		// get pointer to read value
		const auto* const Value = Stack.MostRecentPropertyAddress;

		// end of reading arguments
		P_FINISH;

		// beginning of native processing
		P_NATIVE_BEGIN;

		// Perform fill
		MARK_PROPERTY_DIRTY(Stack.Object, TargetArrayProperty);
		GenericFill(TargetArrayAddr, *TargetArrayProperty, StartIndex, EndIndex,
		            Value);

		// end of native processing
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execFindIf) {
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
		*static_cast<int32*>(RESULT_PARAM) = GenericFindIf(
		    TargetArrayAddr, *TargetArrayProperty, *Object, *Predicate);

		// end of native processing
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execMax) {
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
		// read argument 2 (ComparisonFunctionName) //
		//////////////////////////////////////////////
		P_GET_PROPERTY(FNameProperty, ComparisonFunctionName);

		////////////////////////////////
		// read argument 3 (MaxValue) //
		////////////////////////////////
		// Since MaxValue isn't really an int, step the stack manually

		// reset MostRecentPropertyAddress
		Stack.MostRecentPropertyAddress = nullptr;

		// read a value from Stack
		Stack.StepCompiledIn<FProperty>(nullptr);

		// get pointer to max value ref
		auto* const OutMaxValue = Stack.MostRecentPropertyAddress;

		// end of reading arguments
		P_FINISH;

		// beginning of native processing
		P_NATIVE_BEGIN;

		// get ComparisonFunction on Object
		const auto& ComparisonFunction =
		    Object->FindFunction(ComparisonFunctionName);

		// if ComparisonFunction doesn't exist
		if (!ComparisonFunction) {
			// output error
			UE_LOG(LogUdonArrayUtilsLibrary, Error,
			       TEXT("ComparisonFunction '%s' not found on object: %s"),
			       *ComparisonFunctionName.ToString(), *Object->GetName());

			// finish
			return;
		}

		// get max
		const auto* const MaxElementPtr = GenericMax(
		    TargetArrayAddr, *TargetArrayProperty, *Object, *ComparisonFunction);

		// if max element exists (i.e. array is not empty)
		if (MaxElementPtr) {
			// get element property
			const auto& ElementProperty = TargetArrayProperty->Inner;

			// copy the result to the MaxValue pin
			ElementProperty->CopySingleValueToScriptVM(OutMaxValue, MaxElementPtr);
		}

		// end of native processing
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execMaxElementIndex) {
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

		// if ComparisonFunction doesn't exist
		if (!ComparisonFunction) {
			// output error
			UE_LOG(LogUdonArrayUtilsLibrary, Error,
			       TEXT("ComparisonFunction '%s' not found on object: %s"),
			       *ComparisonFunctionName.ToString(), *Object->GetName());

			// finish
			return;
		}

		// get max
		*static_cast<int32*>(RESULT_PARAM) = GenericMaxElementIndex(
		    TargetArrayAddr, *TargetArrayProperty, *Object, *ComparisonFunction);

		// end of native processing
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execMin) {
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
		// read argument 2 (ComparisonFunctionName) //
		//////////////////////////////////////////////
		P_GET_PROPERTY(FNameProperty, ComparisonFunctionName);

		////////////////////////////////
		// read argument 3 (MinValue) //
		////////////////////////////////
		// Since MinValue isn't really an int, step the stack manually

		// reset MostRecentPropertyAddress
		Stack.MostRecentPropertyAddress = nullptr;

		// read a value from Stack
		Stack.StepCompiledIn<FProperty>(nullptr);

		// get pointer to min value ref
		auto* const OutMinValue = Stack.MostRecentPropertyAddress;

		// end of reading arguments
		P_FINISH;

		// beginning of native processing
		P_NATIVE_BEGIN;

		// get ComparisonFunction on Object
		const auto& ComparisonFunction =
		    Object->FindFunction(ComparisonFunctionName);

		// if ComparisonFunction doesn't exist
		if (!ComparisonFunction) {
			// output error
			UE_LOG(LogUdonArrayUtilsLibrary, Error,
			       TEXT("ComparisonFunction '%s' not found on object: %s"),
			       *ComparisonFunctionName.ToString(), *Object->GetName());

			// finish
			return;
		}

		// get min
		const auto* const MinElementPtr = GenericMin(
		    TargetArrayAddr, *TargetArrayProperty, *Object, *ComparisonFunction);

		// if min element exists (i.e. array is not empty)
		if (MinElementPtr) {
			// get element property
			const auto& ElementProperty = TargetArrayProperty->Inner;

			// copy the result to the MinValue pin
			ElementProperty->CopySingleValueToScriptVM(OutMinValue, MinElementPtr);
		}

		// end of native processing
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execMinElementIndex) {
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

		// if ComparisonFunction doesn't exist
		if (!ComparisonFunction) {
			// output error
			UE_LOG(LogUdonArrayUtilsLibrary, Error,
			       TEXT("ComparisonFunction '%s' not found on object: %s"),
			       *ComparisonFunctionName.ToString(), *Object->GetName());

			// finish
			return;
		}

		// get min
		*static_cast<int32*>(RESULT_PARAM) = GenericMinElementIndex(
		    TargetArrayAddr, *TargetArrayProperty, *Object, *ComparisonFunction);

		// end of native processing
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execNoneSatisfy) {
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

		// Perform the none_of
		*static_cast<bool*>(RESULT_PARAM) = GenericNoneSatisfy(
		    TargetArrayAddr, *TargetArrayProperty, *Object, *Predicate);

		// end of native processing
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execRemoveRange) {
		///////////////////////////////////
		// read argument 0 (TargetArray) //
		///////////////////////////////////

		// reset MostRecentProperty
		Stack.MostRecentProperty = nullptr;

		// read an array from Stack
		Stack.StepCompiledIn<FArrayProperty>(nullptr);

		// get pointer to read array
		void* const TargetArrayAddr = Stack.MostRecentPropertyAddress;

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

		//////////////////////////////////
		// read argument 1 (StartIndex) //
		//////////////////////////////////
		P_GET_PROPERTY(FIntProperty, StartIndex);

		////////////////////////////////
		// read argument 2 (EndIndex) //
		////////////////////////////////
		P_GET_PROPERTY(FIntProperty, EndIndex);

		// end of reading arguments
		P_FINISH;

		// beginning of native processing
		P_NATIVE_BEGIN;

		// Perform remove range
		MARK_PROPERTY_DIRTY(Stack.Object, TargetArrayProperty);
		GenericRemoveRange(TargetArrayAddr, *TargetArrayProperty, StartIndex,
		                   EndIndex);

		// end of native processing
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execRemoveIf) {
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

		// Perform remove_if
		MARK_PROPERTY_DIRTY(Stack.Object, TargetArrayProperty);
		GenericRemoveIf(TargetArrayAddr, *TargetArrayProperty, *Object, *Predicate);

		// end of native processing
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execRandomSample) {
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

		////////////////////////////////////
		// read argument 1 (NumOfSamples) //
		////////////////////////////////////
		P_GET_PROPERTY(FIntProperty, NumOfSamples);

		///////////////////////////////
		// read argument 2 (Samples) //
		///////////////////////////////

		// reset MostRecentProperty
		Stack.MostRecentProperty = nullptr;

		// read an array from Stack
		Stack.StepCompiledIn<FArrayProperty>(nullptr);

		// get pointer to the array
		void* SamplesAddr = Stack.MostRecentPropertyAddress;

		// get property of the array
		FArrayProperty* SamplesProperty =
		    CastField<FArrayProperty>(Stack.MostRecentProperty);

		// if failed to read the array or the array is not same type as TargetArray
		if (!SamplesProperty || !SamplesProperty->SameType(TargetArrayProperty)) {
			// notify that failed
			Stack.bArrayContextFailed = true;

			// finish
			return;
		}

		///////////////////////////////
		// read argument 3 (Others) //
		///////////////////////////////

		// reset MostRecentProperty
		Stack.MostRecentProperty = nullptr;

		// read an array from Stack
		Stack.StepCompiledIn<FArrayProperty>(nullptr);

		// get pointer to the array
		void* OthersAddr = Stack.MostRecentPropertyAddress;

		// get property of the array
		FArrayProperty* OthersProperty =
		    CastField<FArrayProperty>(Stack.MostRecentProperty);

		// if failed to read the array or the array is not same type as TargetArray
		if (!OthersProperty || !OthersProperty->SameType(TargetArrayProperty)) {
			// notify that failed
			Stack.bArrayContextFailed = true;

			// finish
			return;
		}

		// end of reading arguments
		P_FINISH;

		// beginning of native processing
		P_NATIVE_BEGIN;

		// get samples
		const auto& [SamplesSharedPtr, OthersSharedPtr] = GenericRandomSample(
		    TargetArrayAddr, *TargetArrayProperty, NumOfSamples);

		// copy the result to the Samples pin
		SamplesProperty->CopyCompleteValueToScriptVM(SamplesAddr,
		                                             SamplesSharedPtr.get());

		// copy the result to the Samples pin
		SamplesProperty->CopyCompleteValueToScriptVM(OthersAddr,
		                                             OthersSharedPtr.get());

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
		GenericSortAnyArray(TargetArrayAddr, *TargetArrayProperty, *Object,
		                    *ComparisonFunction);

		// end of native processing
		P_NATIVE_END;
	}
};
