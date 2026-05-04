#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "VR_OMNYPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class AKiosk;

UCLASS()
class VR_OMNY_API AVR_OMNYPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AVR_OMNYPlayerController();

	UPROPERTY(BlueprintReadWrite, Category="OMNY")
	TObjectPtr<AKiosk> NearbyKiosk;

protected:

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* Pawn) override;

	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* IA_Interact;

	/** Mapping contexts applied at runtime */
	UPROPERTY(EditAnywhere, Category="Input")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Mapping contexts excluded when using touch controls */
	UPROPERTY(EditAnywhere, Category="Input")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget */
	UPROPERTY(EditAnywhere, Category="Input|Touch")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** Force touch controls regardless of platform */
	UPROPERTY(EditAnywhere, Category="Input|Touch")
	bool bForceTouchControls = false;

	void HandleInteract();

	bool ShouldUseTouchControls() const;
};