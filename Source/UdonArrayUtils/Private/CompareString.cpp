// Copyright 2024 Udon-Tobira, All Rights Reserved.

#include "CompareString.h"

bool UUdonCompareString::Less_StrStr(const FString& A, const FString& B) {
	return FCString::Strcmp(*A, *B) < 0;
}

bool UUdonCompareString::Less_StriStri(const FString& A, const FString& B) {
	return FCString::Stricmp(*A, *B) < 0;
}

bool UUdonCompareString::LessEqual_StrStr(const FString& A, const FString& B) {
	return FCString::Strcmp(*A, *B) <= 0;
}

bool UUdonCompareString::LessEqual_StriStri(const FString& A,
                                            const FString& B) {
	return FCString::Stricmp(*A, *B) <= 0;
}

bool UUdonCompareString::Greater_StrStr(const FString& A, const FString& B) {
	return FCString::Strcmp(*A, *B) > 0;
}

bool UUdonCompareString::Greater_StriStri(const FString& A, const FString& B) {
	return FCString::Stricmp(*A, *B) > 0;
}

bool UUdonCompareString::GreaterEqual_StrStr(const FString& A,
                                             const FString& B) {
	return FCString::Strcmp(*A, *B) >= 0;
}

bool UUdonCompareString::GreaterEqual_StriStri(const FString& A,
                                               const FString& B) {
	return FCString::Stricmp(*A, *B) >= 0;
}
