#include "Station.h"
#include "OMNYExceptions.h"

AStation::AStation()
{
    StationName = TEXT("Unnamed Station");
}

AGate* AStation::GetGate(int32 Index) const
{
    try
    {
        if (!Gates.IsValidIndex(Index))
            throw OutOfRangeInputException(TCHAR_TO_UTF8(*FString::Printf(TEXT("Gate index %d"), Index)));
        return Gates[Index];
    }
    catch (const OutOfRangeInputException& e)
    {
        UE_LOG(LogTemp, Warning, TEXT("AStation::GetGate — %s"), UTF8_TO_TCHAR(e.what()));
        return nullptr;
    }
}

ATimes42ndStation::ATimes42ndStation()
{
    StationName = TEXT("Times Sq - 42 St");
}
