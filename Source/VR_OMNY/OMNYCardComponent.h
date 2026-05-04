#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OMNYCardComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VR_OMNY_API UOMNYCardComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static constexpr float MinAdd      = 0.01f;
	static constexpr float MaxBalance  = 100.f;

	UOMNYCardComponent();

	void InitCard(const FString& InOwnerName, float StartBalance);

	UFUNCTION(BlueprintPure, Category="OMNY Card")
	float GetBalance() const { return Balance; }

	UFUNCTION(BlueprintPure, Category="OMNY Card")
	bool IsValid() const { return bInitialized; }

	// Returns false if the resulting balance would exceed MaxBalance.
	bool AddFunds(float Amount);

	// Returns false if the card has insufficient funds for the fare.
	bool Deduct(float Fare);

private:
	UPROPERTY(VisibleAnywhere, Category="OMNY Card")
	float Balance = 0.f;

	UPROPERTY(VisibleAnywhere, Category="OMNY Card")
	FString OwnerName;

	bool bInitialized = false;
};
