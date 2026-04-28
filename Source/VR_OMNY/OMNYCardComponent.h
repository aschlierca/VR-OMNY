#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OMNYCard.h"
#include "TransactionLog.h"
#include "TInventory.h"
#include <array>
#include "OMNYCardComponent.generated.h"

// ── Enums ────────────────────────────────────────────────────────────────────

UENUM(BlueprintType)
enum class EKioskScreen : uint8
{
    Home,
    AddBalance,
    AddCustom,      // keypad input screen
    BuySingle,
    PayOption,
    PayCard,
    PayCash,
    PayFinal,
    ReadCard,
    DisplayInfo
};

UENUM(BlueprintType)
enum class EPaymentMethod : uint8
{
    None,
    CreditDebit,
    Cash,
    Split
};

UENUM(BlueprintType)
enum class EReadCardContext : uint8
{
    Reload,     // coming from Home → Reload
    CheckInfo   // coming from Home → CheckInfo
};

UENUM(BlueprintType)
enum class EBalanceFlow : uint8
{
    BuyNew,     // AddBalance → PayOption
    BuySingle   // BuySingle  → PayOption
};

// ── Widget class ─────────────────────────────────────────────────────────────

UCLASS()
class VROMNY_API UOMNYCardComponent : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct()  override;

    // Transition to a screen — all navigation goes through here
    UFUNCTION(BlueprintCallable, Category="Kiosk")
    void SetScreen(EKioskScreen NewScreen);

    // Call from AOMNYKioskActor when the player "taps" their card
    UFUNCTION(BlueprintCallable, Category="Kiosk")
    void TriggerCardTap();

    UFUNCTION(BlueprintCallable, Category="Kiosk")
    float GetSessionTotal() const;

    UFUNCTION(BlueprintCallable, Category="Kiosk")
    EKioskScreen GetCurrentScreen() const { return CurrentScreen; }

protected:
    // ── Background image (swaps per screen) ─────────────────────────────────
    UPROPERTY(meta = (BindWidget))
    class UImage* ScreenImage;

    // ── Optional text overlays (BindWidgetOptional = ok if absent) ───────────
    UPROPERTY(meta = (BindWidgetOptional))
    class UTextBlock* Text_BalanceDisplay;   // shows session total / balance buffer

    UPROPERTY(meta = (BindWidgetOptional))
    class UTextBlock* Text_KeypadDisplay;    // shows current keypad input

    UPROPERTY(meta = (BindWidgetOptional))
    class UTextBlock* Text_CardBalance;      // shows card balance on DisplayInfo

    UPROPERTY(meta = (BindWidgetOptional))
    class UTextBlock* Text_FareBreakdown;    // shows fare counts on BuySingle

    // ── Screen textures — assign these in the widget's Class Defaults ────────
    UPROPERTY(EditAnywhere, Category="Screens")
    UTexture2D* Tex_Home;
    UPROPERTY(EditAnywhere, Category="Screens")
    UTexture2D* Tex_AddBalance;
    UPROPERTY(EditAnywhere, Category="Screens")
    UTexture2D* Tex_AddCustom;
    UPROPERTY(EditAnywhere, Category="Screens")
    UTexture2D* Tex_BuySingle;
    UPROPERTY(EditAnywhere, Category="Screens")
    UTexture2D* Tex_PayOption;
    UPROPERTY(EditAnywhere, Category="Screens")
    UTexture2D* Tex_PayCard;
    UPROPERTY(EditAnywhere, Category="Screens")
    UTexture2D* Tex_PayCash;
    UPROPERTY(EditAnywhere, Category="Screens")
    UTexture2D* Tex_PayFinal;
    UPROPERTY(EditAnywhere, Category="Screens")
    UTexture2D* Tex_ReadCard;
    UPROPERTY(EditAnywhere, Category="Screens")
    UTexture2D* Tex_DisplayInfo;

    // ── Home buttons ─────────────────────────────────────────────────────────
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_BuyNew;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_BuySingle;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_Reload;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_CheckInfo;

    // ── AddBalance buttons ───────────────────────────────────────────────────
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_Add10;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_Add20;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_AddKeypad;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_AddFareCalc;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_AddBalanceContinue;

    // ── AddCustom keypad buttons (2D grid: 4 rows × 3 cols) ─────────────────
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_Key1;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_Key2;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_Key3;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_Key4;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_Key5;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_Key6;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_Key7;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_Key8;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_Key9;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_Key0;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_KeyDot;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_KeyBackspace;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_KeyClear;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_KeyConfirm;

    // ── BuySingle buttons ────────────────────────────────────────────────────
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_AddLocal;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_AddExpress;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_BuySingleContinue;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_ClearEntry;

    // ── PayOption buttons ────────────────────────────────────────────────────
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_CreditDebit;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_Cash;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_SplitPayment;

    // ── PayFinal buttons ─────────────────────────────────────────────────────
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_PrintReceipt;

    // ── DisplayInfo buttons ──────────────────────────────────────────────────
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_ReloadCard;
    UPROPERTY(meta = (BindWidget)) class UButton* Btn_ExitInfo;

private:
    // ── Session state ────────────────────────────────────────────────────────
    EKioskScreen     CurrentScreen   = EKioskScreen::Home;
    EReadCardContext ReadCardContext  = EReadCardContext::Reload;
    EPaymentMethod   SelectedPayment = EPaymentMethod::None;
    EBalanceFlow     BalanceFlow     = EBalanceFlow::BuyNew;

    float   BalanceToAdd     = 0.f;
    FString KeypadInput;
    int32   LocalFareCount   = 0;
    int32   ExpressFareCount = 0;

    static constexpr float LOCAL_FARE   = 2.90f;
    static constexpr float EXPRESS_FARE = 6.75f;
    static constexpr float MAX_LOAD     = 100.f;
    static constexpr float MAX_KEYPAD   = 9999.f;

    // ── Domain objects ───────────────────────────────────────────────────────
    TSharedPtr<FOMNYCard> ActiveCard;
    FTransactionLog       TxLog;
    TInventory<FOMNYCard> CardInventory;

    // ── Timer handles ────────────────────────────────────────────────────────
    FTimerHandle PaymentTimerHandle;
    FTimerHandle ReadCardTimerHandle;

    // ── Keypad: 2D grid [row][col] holds digit label (4 rows × 3 cols) ──────
    using FKeypadGrid = std::array<std::array<FString, 3>, 4>;
    FKeypadGrid BuildKeypadGrid() const;
    void        HandleKeypadInput(const FString& Key);
    bool        ValidateKeypadAmount() const;   // numeric, > 0, <= MAX_LOAD

    // ── Screen management ────────────────────────────────────────────────────
    void SwapScreenTexture(UTexture2D* Tex);
    void BuildButtonGroups();
    void RefreshScreen(EKioskScreen Screen);
    void UpdateTextForScreen(EKioskScreen Screen);
    static void SetBtnVisible(UButton* Btn, bool bVisible);

    TMap<EKioskScreen, TArray<class UButton*>> ScreenButtonGroups;

    // ── Timer callbacks ──────────────────────────────────────────────────────
    UFUNCTION() void OnPaymentProcessingDone();
    UFUNCTION() void OnCardDetected();

    // ── Save / load ──────────────────────────────────────────────────────────
    void    SaveProgress() const;
    void    LoadProgress();
    FString GetSaveFilePath() const;

    // ── Button click handlers ────────────────────────────────────────────────
    UFUNCTION() void OnBuyNew();
    UFUNCTION() void OnBuySingleNav();
    UFUNCTION() void OnReload();
    UFUNCTION() void OnCheckInfo();

    UFUNCTION() void OnAdd10();
    UFUNCTION() void OnAdd20();
    UFUNCTION() void OnAddKeypad();
    UFUNCTION() void OnAddFareCalc();
    UFUNCTION() void OnAddBalanceContinue();

    UFUNCTION() void OnAddLocal();
    UFUNCTION() void OnAddExpress();
    UFUNCTION() void OnBuySingleContinue();
    UFUNCTION() void OnClearEntry();

    UFUNCTION() void OnCreditDebit();
    UFUNCTION() void OnCash();
    UFUNCTION() void OnSplitPayment();

    UFUNCTION() void OnPrintReceipt();

    UFUNCTION() void OnReloadCard();
    UFUNCTION() void OnExitInfo();
};
