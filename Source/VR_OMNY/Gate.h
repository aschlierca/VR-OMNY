#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gate.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGateEvent, bool, bSuccess);

UCLASS()
class VR_OMNY_API AGate : public AActor
{
	GENERATED_BODY()

public:
	AGate();

	// Blueprint can bind to this to play green/red flash + sound.
	UPROPERTY(BlueprintAssignable, Category = "Gate")
	FOnGateEvent OnGateTapped;

	// Called when player presses F inside the gate trigger zone.
	UFUNCTION(BlueprintCallable, Category = "Gate")
	void TryOpen(class AVR_OMNYCharacter *Player);

	UFUNCTION(BlueprintPure, Category = "Gate")
	bool IsOpen() const { return bIsOpen; }

protected:
	virtual void BeginPlay() override;

private:
	bool bIsOpen = false;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent *TriggerBox;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent *OverlappedComp, AActor *OtherActor,
						UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent *OverlappedComp, AActor *OtherActor,
					  UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);
};
