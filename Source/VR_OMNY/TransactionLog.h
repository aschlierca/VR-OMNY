#pragma once
#include "CoreMinimal.h"

struct FTransactionEntry
{
	FString Type; // "AddFunds" | "Fare" | "Purchase"
	float Amount;
	float BalanceAfter;
	FString Timestamp;
};

class VR_OMNY_API FTransactionLog
{
public:
	void Record(const FString &Type, float Amount, float BalanceAfter);
	void PrintAll() const;
	const TArray<FTransactionEntry> &GetEntries() const { return Entries; }

private:
	TArray<FTransactionEntry> Entries;
};