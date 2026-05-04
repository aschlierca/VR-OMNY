#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/WidgetSwitcher.h"
#include "OMNYCardComponent.h"
#include "KioskWidget.generated.h"

UENUM(BlueprintType)
enum class EKioskScreen : uint8
{
	Home        UMETA(DisplayName = "Home"),
	AddBalance  UMETA(DisplayName = "AddBalance"),
	AddKeypad   UMETA(DisplayName = "AddKeypad"),
	PayOption   UMETA(DisplayName = "PayOption"),
	PayCard     UMETA(DisplayName = "PayCard"),
	PayCash     UMETA(DisplayName = "PayCash"),
	PayFinal    UMETA(DisplayName = "PayFinal"),
	BuySingle   UMETA(DisplayName = "BuySingle"),
	ReadCard    UMETA(DisplayName = "ReadCard"),
	DisplayInfo UMETA(DisplayName = "DisplayInfo"),
};

UENUM()
enum class EKioskFlow : uint8
{
	None,
	BuyNew,
	Reload,
	CheckInfo,
	BuySingle
};

UCLASS()
class VR_OMNY_API UKioskWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// ── Bound widgets ─────────────────────────────────────────────────────────
	UPROPERTY(meta = (BindWidget))
	UImage *Img_Screen;

	// ── Widget Switcher — drives which screen page is visible ─────────────────
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher *WS_Screen;

	// ── Screen panels — direct children of WS_Screen, name them to match ─────
	UPROPERTY(meta = (BindWidgetOptional)) UPanelWidget *HomePanel;
	UPROPERTY(meta = (BindWidgetOptional)) UPanelWidget *AddBalancePanel;
	UPROPERTY(meta = (BindWidgetOptional)) UPanelWidget *AddKeypadPanel;
	UPROPERTY(meta = (BindWidgetOptional)) UPanelWidget *PayOptionPanel;
	UPROPERTY(meta = (BindWidgetOptional)) UPanelWidget *PayCardPanel;
	UPROPERTY(meta = (BindWidgetOptional)) UPanelWidget *PayCashPanel;
	UPROPERTY(meta = (BindWidgetOptional)) UPanelWidget *PayFinalPanel;
	UPROPERTY(meta = (BindWidgetOptional)) UPanelWidget *BuySinglePanel;
	UPROPERTY(meta = (BindWidgetOptional)) UPanelWidget *ReadCardPanel;
	UPROPERTY(meta = (BindWidgetOptional)) UPanelWidget *DisplayInfoPanel;

	// Home screen buttons
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_BuyNew;
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_BuySingle;
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_Reload;
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_CheckInfo;

	// AddBalance screen buttons
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_Add10;
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_Add20;
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_AddKeypad;
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_AddFareCalc;

	// PayOption screen buttons
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_CreditDebit;
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_Cash;
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_SplitPayment;

	// PayFinal screen buttons
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_PrintReceipt;

	// BuySingle screen buttons
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_AddLocal;
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_AddExpress;
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_Continue;
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_ClearEntry;

	// DisplayInfo screen buttons
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_ReloadCard;
	UPROPERTY(meta = (BindWidget))
	UButton *Btn_ExitInfo;

	// ── Screen textures — assign PNGs in Blueprint Details ───────────────────
	// Key: screen enum value. Value: the imported PNG texture for that screen.
	// ── Screen textures — assign one PNG per EKioskScreen value in Blueprint ──
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kiosk|Screens")
	TMap<EKioskScreen, UTexture2D *> ScreenTextures;

	// ── Setup ─────────────────────────────────────────────────────────────────
	// AKiosk calls this before AddToViewport.
	UFUNCTION(BlueprintCallable, Category = "Kiosk")
	void SetCardComponent(UOMNYCardComponent *InCard) { CardComponent = InCard; }

	// ── Navigation ────────────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category = "Kiosk|Nav")
	void GoToScreen(EKioskScreen Screen);

	UFUNCTION(BlueprintPure, Category = "Kiosk|Nav")
	EKioskScreen GetCurrentScreen() const { return CurrentScreen; }

	// ── Home buttons ──────────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category = "Kiosk|Home")
	void OnBuyNew(); // → AddBalance

	UFUNCTION(BlueprintCallable, Category = "Kiosk|Home")
	void OnBuySingle(); // → BuySingle

	UFUNCTION(BlueprintCallable, Category = "Kiosk|Home")
	void OnReload(); // → ReadCard → AddBalance

	UFUNCTION(BlueprintCallable, Category = "Kiosk|Home")
	void OnCheckInfo(); // → ReadCard → DisplayInfo

	// ── AddBalance buttons ────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category = "Kiosk|AddBalance")
	void OnAdd10();

	UFUNCTION(BlueprintCallable, Category = "Kiosk|AddBalance")
	void OnAdd20();

	// Button click: navigates to the custom keypad entry screen.
	UFUNCTION(BlueprintCallable, Category = "Kiosk|AddBalance")
	void OnAddKeypadBtn(); // → AddKeypad screen

	// Called from Blueprint once the user confirms a custom amount on the keypad screen.
	UFUNCTION(BlueprintCallable, Category = "Kiosk|AddBalance")
	void OnAddKeypad(float Amount); // → PayOption

	UFUNCTION(BlueprintCallable, Category = "Kiosk|AddBalance")
	void OnAddFareCalc(); // → BuySingle

	// ── PayOption buttons ─────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category = "Kiosk|PayOption")
	void OnCreditDebit(); // → PayCard

	UFUNCTION(BlueprintCallable, Category = "Kiosk|PayOption")
	void OnCash(); // → PayCash

	UFUNCTION(BlueprintCallable, Category = "Kiosk|PayOption")
	void OnSplitPayment(); // → PayCard (simplified)

	// ── PayCard / PayCash — no physical buttons ───────────────────────────────
	// Wire a Blueprint timer (e.g. 3 s) on these screens to call this.
	UFUNCTION(BlueprintCallable, Category = "Kiosk|Payment")
	void OnPaymentProcessed(); // commits funds → PayFinal

	// ── PayFinal buttons ──────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category = "Kiosk|PayFinal")
	void OnPrintReceipt(); // → Home

	// ── BuySingle buttons ─────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category = "Kiosk|BuySingle")
	void OnAddLocal(); // adds $2.90 to running total

	UFUNCTION(BlueprintCallable, Category = "Kiosk|BuySingle")
	void OnAddExpress(); // adds $7.75 to running total

	UFUNCTION(BlueprintCallable, Category = "Kiosk|BuySingle")
	void OnBuySingleContinue(); // validates total > 0 → PayOption

	UFUNCTION(BlueprintCallable, Category = "Kiosk|BuySingle")
	void OnClearEntry(); // resets running total to zero

	// ── ReadCard — no physical buttons ───────────────────────────────────────
	// Wire a Blueprint timer (e.g. 1.5 s) to call this automatically.
	UFUNCTION(BlueprintCallable, Category = "Kiosk|ReadCard")
	void SimulateCardRead();

	// ── DisplayInfo buttons ───────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category = "Kiosk|DisplayInfo")
	void OnReloadCard(); // → AddBalance

	UFUNCTION(BlueprintCallable, Category = "Kiosk|DisplayInfo")
	void OnExitInfo(); // → Home

	// ── Blueprint events (implement in WBP_KioskWidget Event Graph) ──────────
	UFUNCTION(BlueprintImplementableEvent, Category = "Kiosk")
	void OnScreenChanged(EKioskScreen NewScreen);

	UFUNCTION(BlueprintImplementableEvent, Category = "Kiosk")
	void OnBalanceUpdated(float NewBalance);

	UFUNCTION(BlueprintImplementableEvent, Category = "Kiosk")
	void OnTotalUpdated(float NewTotal);

	UFUNCTION(BlueprintImplementableEvent, Category = "Kiosk")
	void OnError(const FString &ErrorMessage);

	// ── State queries ─────────────────────────────────────────────────────────
	UFUNCTION(BlueprintPure, Category = "Kiosk")
	float GetPendingAmount() const { return PendingAmount; }

	UFUNCTION(BlueprintPure, Category = "Kiosk")
	float GetBuySingleTotal() const { return BuySingleTotal; }

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	UOMNYCardComponent *CardComponent = nullptr;

	EKioskScreen CurrentScreen = EKioskScreen::Home;
	EKioskFlow CurrentFlow = EKioskFlow::None;

	float PendingAmount = 0.f;
	float BuySingleTotal = 0.f;
	int32 LocalRideCount = 0;
	int32 ExpressRideCount = 0;

	static constexpr float LocalFare = 2.90f;
	static constexpr float ExpressFare = 7.75f;

	// Disables every button and hides every panel, then shows only the active one.
	void RefreshButtonStates();

	void ShowOnlyPanel(UPanelWidget* Active);
	void ApplyScreenTexture(EKioskScreen Screen);

	bool ValidateAmount(float Amount) const;
	void CommitFunds();
};
