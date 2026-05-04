// Copyright Epic Games, Inc. All Rights Reserved.

#include "VR_OMNYPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "VR_OMNYCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "VR_OMNY.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "VR_OMNYCharacter.h"
#include "Gate.h"
#include "Kiosk.h"
#include "Kismet/GameplayStatics.h"

AVR_OMNYPlayerController::AVR_OMNYPlayerController()
{
	PlayerCameraManagerClass = AVR_OMNYCameraManager::StaticClass();
}

void AVR_OMNYPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Mobile UI only
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			MobileControlsWidget->AddToPlayerScreen(0);
		}
		else
		{
			UE_LOG(LogVR_OMNY, Error, TEXT("Could not spawn mobile controls widget."));
		}
	}
}

void AVR_OMNYPlayerController::OnPossess(APawn* Pawn)
{
	Super::OnPossess(Pawn);

	// Add Input Mapping Contexts when possession happens (BEST PRACTICE)
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* Ctx : DefaultMappingContexts)
		{
			if (Ctx)
			{
				Subsystem->AddMappingContext(Ctx, 0);
			}
		}

		if (!ShouldUseTouchControls())
		{
			for (UInputMappingContext* Ctx : MobileExcludedMappingContexts)
			{
				if (Ctx)
				{
					Subsystem->AddMappingContext(Ctx, 0);
				}
			}
		}
	}
}

void AVR_OMNYPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!IsLocalPlayerController())
		return;

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (IA_Interact)
			EIC->BindAction(IA_Interact, ETriggerEvent::Started, this, &AVR_OMNYPlayerController::HandleInteract);
	}
}

void AVR_OMNYPlayerController::HandleInteract()
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
		return;

	// Resolve nearest kiosk: overlap callback, then character cache, then live scan
	AKiosk* Kiosk = NearbyKiosk;
	if (!Kiosk)
		if (AVR_OMNYCharacter* Player = Cast<AVR_OMNYCharacter>(MyPawn))
			Kiosk = Player->NearbyKiosk;
	if (!Kiosk)
	{
		TArray<AActor*> AllKiosks;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AKiosk::StaticClass(), AllKiosks);
		for (AActor* A : AllKiosks)
			if (A->IsOverlappingActor(MyPawn)) { Kiosk = Cast<AKiosk>(A); break; }
	}

	if (Kiosk)
	{
		if (Kiosk->IsWidgetOpen())
			Kiosk->CloseWidget();
		else
			Kiosk->OpenWidget(this);
		return;
	}

	if (AVR_OMNYCharacter* Player = Cast<AVR_OMNYCharacter>(MyPawn))
		if (Player->bNearGate && Player->NearbyGate)
			Player->NearbyGate->TryOpen(Player);
}

bool AVR_OMNYPlayerController::ShouldUseTouchControls() const
{
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}