#pragma once
#include "CoreMinimal.h"
#include <stdexcept>

// ── Custom exceptions ────────────────────────────────────────────────────────

class FInsufficientFundsException : public std::exception
{
public:
    FInsufficientFundsException(float InBalance, float InRequired)
        : Balance(InBalance), Required(InRequired) {}

    const char *what() const noexcept override { return "Insufficient funds on OMNY card"; }
    float GetBalance() const { return Balance; }
    float GetRequired() const { return Required; }

private:
    float Balance;
    float Required;
};

class FInvalidCardException : public std::exception
{
public:
    const char *what() const noexcept override { return "OMNY card is invalid or uninitialized"; }
};

// ── Card class ───────────────────────────────────────────────────────────────

class VROMNY_API FOMNYCard
{
public:
    FOMNYCard() : CardID(TEXT("")), Balance(0.f), bValid(false) {}

    FOMNYCard(const FString &InID, float InitBalance)
        : CardID(InID), Balance(InitBalance), bValid(true)
    {
        if (InitBalance < 0.f)
            throw std::invalid_argument("Initial balance cannot be negative");
    }

    float GetBalance() const { return Balance; }
    FString GetCardID() const { return CardID; }
    bool IsValid() const { return bValid; }

    void AddFunds(float Amount)
    {
        if (!bValid)
            throw FInvalidCardException();
        if (Amount <= 0.f)
            throw std::invalid_argument("Amount must be greater than zero");
        Balance += Amount;
    }

    void DeductFare(float Fare)
    {
        if (!bValid)
            throw FInvalidCardException();
        if (Balance < Fare)
            throw FInsufficientFundsException(Balance, Fare);
        Balance -= Fare;
    }

    void Invalidate() { bValid = false; }

private:
    FString CardID;
    float Balance;
    bool bValid;
};
