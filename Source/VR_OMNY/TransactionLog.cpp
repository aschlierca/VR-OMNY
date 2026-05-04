#include "TransactionLog.h"
#include "Misc/DateTime.h"

void FTransactionLog::Record(const FString &Type, float Amount, float BalanceAfter)
{
    FTransactionEntry Entry;
    Entry.Type = Type;
    Entry.Amount = Amount;
    Entry.BalanceAfter = BalanceAfter;
    Entry.Timestamp = FDateTime::Now().ToString();
    Entries.Add(Entry);
}

void FTransactionLog::PrintAll() const
{
    for (const FTransactionEntry &E : Entries)
    {
        UE_LOG(LogTemp, Log, TEXT("[%s] %s | $%.2f | Balance: $%.2f"),
               *E.Timestamp, *E.Type, E.Amount, E.BalanceAfter);
    }
}
