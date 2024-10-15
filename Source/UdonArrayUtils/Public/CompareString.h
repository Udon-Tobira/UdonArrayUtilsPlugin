// Copyright 2024 Udon-Tobira, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "CompareString.generated.h"

/**
 *
 */
UCLASS()
class UDONARRAYUTILS_API UUdonCompareString: public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure,
	          meta     = (DisplayName      = "Less Exactly (String)",
                    CompactNodeTitle = "< (Exactly)", BlueprintThreadSafe),
	          Category = "Utilities|String")
	static bool Less_StrStr(const FString& A, const FString& B);

	UFUNCTION(BlueprintPure,
	          meta     = (DisplayName      = "Less, Case Insensitive (String)",
                    CompactNodeTitle = "<", BlueprintThreadSafe),
	          Category = "Utilities|String")
	static bool Less_StriStri(const FString& A, const FString& B);

	UFUNCTION(BlueprintPure,
	          meta     = (DisplayName      = "Less Equal Exactly (String)",
                    CompactNodeTitle = "<= (Exactly)", BlueprintThreadSafe),
	          Category = "Utilities|String")
	static bool LessEqual_StrStr(const FString& A, const FString& B);

	UFUNCTION(BlueprintPure,
	          meta     = (DisplayName      = "Less Equal, Case Insensitive (String)",
                    CompactNodeTitle = "<=", BlueprintThreadSafe),
	          Category = "Utilities|String")
	static bool LessEqual_StriStri(const FString& A, const FString& B);

	UFUNCTION(BlueprintPure,
	          meta     = (DisplayName      = "Greater Exactly (String)",
                    CompactNodeTitle = "> (Exactly)", BlueprintThreadSafe),
	          Category = "Utilities|String")
	static bool Greater_StrStr(const FString& A, const FString& B);

	UFUNCTION(BlueprintPure,
	          meta     = (DisplayName      = "Greater, Case Insensitive (String)",
                    CompactNodeTitle = ">", BlueprintThreadSafe),
	          Category = "Utilities|String")
	static bool Greater_StriStri(const FString& A, const FString& B);

	UFUNCTION(BlueprintPure,
	          meta     = (DisplayName      = "Greater Equal Exactly (String)",
                    CompactNodeTitle = ">= (Exactly)", BlueprintThreadSafe),
	          Category = "Utilities|String")
	static bool GreaterEqual_StrStr(const FString& A, const FString& B);

	UFUNCTION(BlueprintPure,
	          meta = (DisplayName = "Greater Equal, Case Insensitive (String)",
	                  CompactNodeTitle = ">=", BlueprintThreadSafe),
	          Category = "Utilities|String")
	static bool GreaterEqual_StriStri(const FString& A, const FString& B);
};
