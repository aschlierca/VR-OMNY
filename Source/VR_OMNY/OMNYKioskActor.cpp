#include "OMNYKioskActor.h"
#include "OMNYCardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

AOMNYKioskActor::AOMNYKioskActor()
{
    PrimaryActorTick.bCanEverTick = false;

    KioskMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KioskMesh"));
    SetRootComponent(KioskMesh);

    InteractZone = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractZone"));
    InteractZone->SetupAttachment(KioskMesh);
    InteractZone->SetBoxExtent(FVector(150.f, 150.f, 150.f));
    InteractZone->SetCollisionProfileName(TEXT("Trigger"));
}

void AOMNYKioskActor::BeginPlay()
{
    Super::BeginPlay();

    InteractZone->OnComponentBeginOverlap.AddDynamic(this, &AOMNYKioskActor::OnPlayerEnterZone);
    InteractZone->OnComponentEndOverlap.AddDynamic(this,  &AOMNYKioskActor::OnPlayerExitZone);
}

void AOMNYKioskActor::OpenKiosk()
{
    if (bKioskOpen || !KioskWidgetClass) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    KioskWidget = CreateWidget<UOMNYCardComponent>(PC, KioskWidgetClass);
    if (!KioskWidget) return;

    KioskWidget->AddToViewport(10);
    PC->SetInputMode(FInputModeUIOnly());
    PC->bShowMouseCursor = true;
    bKioskOpen = true;
}

void AOMNYKioskActor::CloseKiosk()
{
    if (!bKioskOpen || !KioskWidget) return;

    KioskWidget->RemoveFromParent();
    KioskWidget = nullptr;

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
    }
    bKioskOpen = false;
}

void AOMNYKioskActor::OnPlayerEnterZone(UPrimitiveComponent*, AActor* OtherActor,
    UPrimitiveComponent*, int32, bool, const FHitResult&)
{
    if (OtherActor && OtherActor->ActorHasTag(TEXT("Player")))
        OpenKiosk();
}

void AOMNYKioskActor::OnPlayerExitZone(UPrimitiveComponent*, AActor* OtherActor,
    UPrimitiveComponent*, int32)
{
    if (OtherActor && OtherActor->ActorHasTag(TEXT("Player")))
        CloseKiosk();
}
