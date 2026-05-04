#include "OMNYCardComponent.h"

UOMNYCardComponent::UOMNYCardComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOMNYCardComponent::InitCard(const FString& InOwnerName, float StartBalance)
{
	OwnerName    = InOwnerName;
	Balance      = FMath::Clamp(StartBalance, 0.f, MaxBalance);
	bInitialized = true;
}

bool UOMNYCardComponent::AddFunds(float Amount)
{
	if (Balance + Amount > MaxBalance)
		return false;
	Balance += Amount;
	return true;
}

bool UOMNYCardComponent::Deduct(float Fare)
{
	if (Balance < Fare)
		return false;
	Balance -= Fare;
	return true;
}
