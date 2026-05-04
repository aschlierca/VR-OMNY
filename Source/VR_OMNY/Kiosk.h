#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kiosk.generated.h"

class UBoxComponent;
class UUserWidget;

UCLASS()
class VR_OMNY_API AKiosk : public AActor
{
    GENERATED_BODY()

public:
    AKiosk();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kiosk")
    TSubclassOf<UUserWidget> KioskWidgetClass;

    UFUNCTION(BlueprintCallable, Category="Kiosk")
    void OpenWidget(APlayerController* PC);

    UFUNCTION(BlueprintCallable, Category="Kiosk")
    void CloseWidget();

    UFUNCTION(BlueprintPure, Category="Kiosk")
    bool IsWidgetOpen() const { return ActiveWidget != nullptr; }

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    UUserWidget* ActiveWidget = nullptr;

    UPROPERTY(VisibleAnywhere, Category="Kiosk")
    UBoxComponent* InteractTrigger;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};