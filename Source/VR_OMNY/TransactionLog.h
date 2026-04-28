#pragma once
#include "CoreMinimal.h"
#include <vector>

UENUM()
enum class ETransactionType : uint8
{
    Purchase,
    Reload,
    FarePaid,
    Refund
};

struct FTransaction
{
    ETransactionType Type;
    float            Amount;
    float            BalanceAfter;
    FString          Timestamp;
};

class VROMNY_API FTransactionLog
{
public:
    void AddEntry(ETransactionType Type, float Amount, float BalanceAfter);

    const std::vector<FTransaction>& GetAll() const { return Entries; }
    int32 Count() const { return static_cast<int32>(Entries.size()); }
    void  Clear()       { Entries.clear(); }

    void SaveToFile(const FString& FilePath) const;
    bool LoadFromFile(const FString& FilePath);   // returns false if file not found

private:
    std::vector<FTransaction> Entries;
    static FString TypeToString(ETransactionType T);
    static ETransactionType StringToType(const FString& S);
};
