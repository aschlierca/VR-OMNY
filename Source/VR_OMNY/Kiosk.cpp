#include "Kiosk.h"
#include "KioskWidget.h"
#include "VR_OMNYCharacter.h"
#include "VR_OMNYPlayerController.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AKiosk::AKiosk()
{
	InteractTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractTrigger"));
	RootComponent = InteractTrigger;
	InteractTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	InteractTrigger->SetGenerateOverlapEvents(true);
	InteractTrigger->OnComponentBeginOverlap.AddDynamic(this, &AKiosk::OnOverlapBegin);
	InteractTrigger->OnComponentEndOverlap.AddDynamic(this, &AKiosk::OnOverlapEnd);
}

void AKiosk::BeginPlay()
{
	Super::BeginPlay();
}

void AKiosk::OpenWidget(APlayerController *PC)
{
	if (!KioskWidgetClass || !PC || ActiveWidget)
		return;

	ActiveWidget = CreateWidget<UUserWidget>(PC, KioskWidgetClass);
	if (!ActiveWidget)
		return;

	// Pass card component if the widget is a full KioskWidget
	if (UKioskWidget* KW = Cast<UKioskWidget>(ActiveWidget))
	{
		if (AVR_OMNYCharacter *Player = Cast<AVR_OMNYCharacter>(PC->GetPawn()))
			KW->SetCardComponent(Player->OMNYCard);
	}

	ActiveWidget->AddToViewport();

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(ActiveWidget->TakeWidget());
	InputMode.SetHideCursorDuringCapture(false);
	PC->SetInputMode(InputMode);
	PC->SetShowMouseCursor(true);

	if (ACharacter* Char = Cast<ACharacter>(PC->GetPawn()))
		Char->GetCharacterMovement()->DisableMovement();
}

void AKiosk::CloseWidget()
{
	if (!ActiveWidget)
		return;

	ActiveWidget->RemoveFromParent();
	ActiveWidget = nullptr;

	if (APlayerController *PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());

		if (ACharacter* Char = Cast<ACharacter>(PC->GetPawn()))
			Char->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void AKiosk::OnOverlapBegin(UPrimitiveComponent *, AActor *OtherActor,
							UPrimitiveComponent *, int32, bool, const FHitResult &)
{
	APawn *Pawn = Cast<APawn>(OtherActor);
	if (!Pawn)
		return;

	// Store on controller so HandleInteract works with any pawn class
	if (AVR_OMNYPlayerController *PC = Cast<AVR_OMNYPlayerController>(Pawn->GetController()))
		PC->NearbyKiosk = this;

	// Also set on AVR_OMNYCharacter if that's the pawn being used
	if (AVR_OMNYCharacter *Player = Cast<AVR_OMNYCharacter>(Pawn))
	{
		Player->bNearKiosk = true;
		Player->NearbyKiosk = this;
	}
}

void AKiosk::OnOverlapEnd(UPrimitiveComponent *, AActor *OtherActor,
						  UPrimitiveComponent *, int32)
{
	APawn *Pawn = Cast<APawn>(OtherActor);
	if (!Pawn)
		return;

	if (AVR_OMNYPlayerController *PC = Cast<AVR_OMNYPlayerController>(Pawn->GetController()))
		PC->NearbyKiosk = nullptr;

	if (AVR_OMNYCharacter *Player = Cast<AVR_OMNYCharacter>(Pawn))
	{
		Player->bNearKiosk = false;
		Player->NearbyKiosk = nullptr;
	}

	CloseWidget();
}
