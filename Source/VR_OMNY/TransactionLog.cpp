#include "TransactionLog.h"
#include "Misc/FileHelper.h"
#include "Misc/DateTime.h"
#include "Misc/Paths.h"

void FTransactionLog::AddEntry(ETransactionType Type, float Amount, float BalanceAfter)
{
    FTransaction T;
    T.Type         = Type;
    T.Amount       = Amount;
    T.BalanceAfter = BalanceAfter;
    T.Timestamp    = FDateTime::Now().ToString(TEXT("%Y-%m-%d %H:%M:%S"));
    Entries.push_back(T);
}

void FTransactionLog::SaveToFile(const FString& FilePath) const
{
    FString Content;
    for (const FTransaction& T : Entries)
    {
        Content += FString::Printf(TEXT("%s,%.2f,%.2f,%s\n"),
            *TypeToString(T.Type), T.Amount, T.BalanceAfter, *T.Timestamp);
    }
    FFileHelper::SaveStringToFile(Content, *FilePath);
}

bool FTransactionLog::LoadFromFile(const FString& FilePath)
{
    FString Content;
    if (!FFileHelper::LoadFileToString(Content, *FilePath))
        return false;   // file not found — caller starts fresh

    Entries.clear();
    TArray<FString> Lines;
    Content.ParseIntoArrayLines(Lines);

    for (const FString& Line : Lines)
    {
        if (Line.IsEmpty()) continue;
        TArray<FString> Parts;
        Line.ParseIntoArray(Parts, TEXT(","));
        if (Parts.Num() < 4) continue;

        try
        {
            FTransaction T;
            T.Type         = StringToType(Parts[0]);
            T.Amount       = FCString::Atof(*Parts[1]);
            T.BalanceAfter = FCString::Atof(*Parts[2]);
            T.Timestamp    = Parts[3];
            Entries.push_back(T);
        }
        catch (...) { /* skip malformed line */ }
    }
    return true;
}

FString FTransactionLog::TypeToString(ETransactionType T)
{
    switch (T)
    {
    case ETransactionType::Purchase: return TEXT("Purchase");
    case ETransactionType::Reload:   return TEXT("Reload");
    case ETransactionType::FarePaid: return TEXT("FarePaid");
    default:                         return TEXT("Refund");
    }
}

ETransactionType FTransactionLog::StringToType(const FString& S)
{
    if (S == TEXT("Purchase")) return ETransactionType::Purchase;
    if (S == TEXT("Reload"))   return ETransactionType::Reload;
    if (S == TEXT("FarePaid")) return ETransactionType::FarePaid;
    return ETransactionType::Refund;
}
