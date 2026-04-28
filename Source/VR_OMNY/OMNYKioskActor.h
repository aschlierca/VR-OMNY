#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OMNYKioskActor.generated.h"

UCLASS()
class VROMNY_API AOMNYKioskActor : public AActor
{
    GENERATED_BODY()

public:
    AOMNYKioskActor();
    virtual void BeginPlay() override;

    // Set this to OMNY_Widget in the editor
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Kiosk")
    TSubclassOf<class UOMNYCardComponent> KioskWidgetClass;

    UFUNCTION(BlueprintCallable, Category="Kiosk")
    void OpenKiosk();

    UFUNCTION(BlueprintCallable, Category="Kiosk")
    void CloseKiosk();

    UFUNCTION(BlueprintCallable, Category="Kiosk")
    bool IsKioskOpen() const { return bKioskOpen; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Kiosk")
    class UStaticMeshComponent* KioskMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Kiosk")
    class UBoxComponent* InteractZone;

private:
    UPROPERTY()
    class UOMNYCardComponent* KioskWidget;

    bool bKioskOpen = false;

    UFUNCTION()
    void OnPlayerEnterZone(UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPlayerExitZone(UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex);
};
