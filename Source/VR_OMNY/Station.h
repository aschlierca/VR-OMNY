#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gate.h"
#include "Station.generated.h"

// Base class — subclass per real station name.
UCLASS()
class VR_OMNY_API AStation : public AActor
{
	GENERATED_BODY()

public:
	AStation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Station")
	FString StationName;

	// Populated in Blueprint by dragging gate actors into this array.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Station")
	TArray<AGate *> Gates;

	// Returns the gate at Index, or nullptr with a logged warning if out of range.
	UFUNCTION(BlueprintCallable, Category = "Station")
	AGate *GetGate(int32 Index) const;

	UFUNCTION(BlueprintPure, Category = "Station")
	int32 GateCount() const { return Gates.Num(); }
};

// Concrete subclass — add more as needed.
UCLASS()
class VR_OMNY_API ATimes42ndStation : public AStation
{
	GENERATED_BODY()
public:
	ATimes42ndStation();
};
