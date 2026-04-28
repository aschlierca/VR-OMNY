#include "OMNYCardComponent.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Slate/SlateBrushAsset.h"
#include "Misc/Paths.h"
#include "TimerManager.h"

// ── Lifecycle ────────────────────────────────────────────────────────────────

void UOMNYCardComponent::NativeConstruct()
{
    Super::NativeConstruct();

    LoadProgress();
    BuildButtonGroups();

    // ── Home ──
    if (Btn_BuyNew)    Btn_BuyNew->OnClicked.AddDynamic(this,    &UOMNYCardComponent::OnBuyNew);
    if (Btn_BuySingle) Btn_BuySingle->OnClicked.AddDynamic(this, &UOMNYCardComponent::OnBuySingleNav);
    if (Btn_Reload)    Btn_Reload->OnClicked.AddDynamic(this,    &UOMNYCardComponent::OnReload);
    if (Btn_CheckInfo) Btn_CheckInfo->OnClicked.AddDynamic(this, &UOMNYCardComponent::OnCheckInfo);

    // ── AddBalance ──
    if (Btn_Add10)              Btn_Add10->OnClicked.AddDynamic(this,              &UOMNYCardComponent::OnAdd10);
    if (Btn_Add20)              Btn_Add20->OnClicked.AddDynamic(this,              &UOMNYCardComponent::OnAdd20);
    if (Btn_AddKeypad)          Btn_AddKeypad->OnClicked.AddDynamic(this,          &UOMNYCardComponent::OnAddKeypad);
    if (Btn_AddFareCalc)        Btn_AddFareCalc->OnClicked.AddDynamic(this,        &UOMNYCardComponent::OnAddFareCalc);
    if (Btn_AddBalanceContinue) Btn_AddBalanceContinue->OnClicked.AddDynamic(this, &UOMNYCardComponent::OnAddBalanceContinue);

    // ── AddCustom keypad — lambdas to avoid 14 separate UFUNCTIONs ──
    auto BindKey = [this](UButton* Btn, const FString& Label)
    {
        if (Btn) Btn->OnClicked.AddWeakLambda(this,
            [this, Label]() { HandleKeypadInput(Label); });
    };
    BindKey(Btn_Key1, TEXT("1")); BindKey(Btn_Key2, TEXT("2")); BindKey(Btn_Key3, TEXT("3"));
    BindKey(Btn_Key4, TEXT("4")); BindKey(Btn_Key5, TEXT("5")); BindKey(Btn_Key6, TEXT("6"));
    BindKey(Btn_Key7, TEXT("7")); BindKey(Btn_Key8, TEXT("8")); BindKey(Btn_Key9, TEXT("9"));
    BindKey(Btn_Key0, TEXT("0")); BindKey(Btn_KeyDot, TEXT("."));
    if (Btn_KeyBackspace) Btn_KeyBackspace->OnClicked.AddWeakLambda(this, [this]()
    {
        if (!KeypadInput.IsEmpty())
            KeypadInput.RemoveAt(KeypadInput.Len() - 1);
        UpdateTextForScreen(CurrentScreen);
    });
    if (Btn_KeyClear)   Btn_KeyClear->OnClicked.AddDynamic(this,   &UOMNYCardComponent::OnBuySingleContinue);   // routed below
    if (Btn_KeyConfirm) Btn_KeyConfirm->OnClicked.AddDynamic(this, &UOMNYCardComponent::OnAddBalanceContinue);  // confirm → PayOption

    // Re-bind KeyClear properly (clears keypad input)
    if (Btn_KeyClear)
    {
        Btn_KeyClear->OnClicked.Clear();
        Btn_KeyClear->OnClicked.AddWeakLambda(this, [this]()
        {
            KeypadInput.Empty();
            UpdateTextForScreen(CurrentScreen);
        });
    }
    // Re-bind KeyConfirm properly (validates and commits)
    if (Btn_KeyConfirm)
    {
        Btn_KeyConfirm->OnClicked.Clear();
        Btn_KeyConfirm->OnClicked.AddWeakLambda(this, [this]()
        {
            if (!ValidateKeypadAmount())
            {
                if (Text_KeypadDisplay)
                    Text_KeypadDisplay->SetText(FText::FromString(TEXT("Invalid amount")));
                return;
            }
            BalanceToAdd += FCString::Atof(*KeypadInput);
            BalanceToAdd = FMath::Min(BalanceToAdd, MAX_LOAD);
            KeypadInput.Empty();
            SetScreen(EKioskScreen::AddBalance);
        });
    }

    // ── BuySingle ──
    if (Btn_AddLocal)          Btn_AddLocal->OnClicked.AddDynamic(this,          &UOMNYCardComponent::OnAddLocal);
    if (Btn_AddExpress)        Btn_AddExpress->OnClicked.AddDynamic(this,        &UOMNYCardComponent::OnAddExpress);
    if (Btn_BuySingleContinue) Btn_BuySingleContinue->OnClicked.AddDynamic(this, &UOMNYCardComponent::OnBuySingleContinue);
    if (Btn_ClearEntry)        Btn_ClearEntry->OnClicked.AddDynamic(this,        &UOMNYCardComponent::OnClearEntry);

    // ── PayOption ──
    if (Btn_CreditDebit)  Btn_CreditDebit->OnClicked.AddDynamic(this,  &UOMNYCardComponent::OnCreditDebit);
    if (Btn_Cash)         Btn_Cash->OnClicked.AddDynamic(this,         &UOMNYCardComponent::OnCash);
    if (Btn_SplitPayment) Btn_SplitPayment->OnClicked.AddDynamic(this, &UOMNYCardComponent::OnSplitPayment);

    // ── PayFinal ──
    if (Btn_PrintReceipt) Btn_PrintReceipt->OnClicked.AddDynamic(this, &UOMNYCardComponent::OnPrintReceipt);

    // ── DisplayInfo ──
    if (Btn_ReloadCard) Btn_ReloadCard->OnClicked.AddDynamic(this, &UOMNYCardComponent::OnReloadCard);
    if (Btn_ExitInfo)   Btn_ExitInfo->OnClicked.AddDynamic(this,   &UOMNYCardComponent::OnExitInfo);

    SetScreen(EKioskScreen::Home);
}

void UOMNYCardComponent::NativeDestruct()
{
    if (UWorld* W = GetWorld())
    {
        W->GetTimerManager().ClearTimer(PaymentTimerHandle);
        W->GetTimerManager().ClearTimer(ReadCardTimerHandle);
    }
    SaveProgress();
    Super::NativeDestruct();
}

// ── Screen management ─────────────────────────────────────────────────────────

void UOMNYCardComponent::BuildButtonGroups()
{
    ScreenButtonGroups.Add(EKioskScreen::Home,
        { Btn_BuyNew, Btn_BuySingle, Btn_Reload, Btn_CheckInfo });

    ScreenButtonGroups.Add(EKioskScreen::AddBalance,
        { Btn_Add10, Btn_Add20, Btn_AddKeypad, Btn_AddFareCalc, Btn_AddBalanceContinue });

    ScreenButtonGroups.Add(EKioskScreen::AddCustom,
        { Btn_Key1, Btn_Key2, Btn_Key3, Btn_Key4, Btn_Key5, Btn_Key6,
          Btn_Key7, Btn_Key8, Btn_Key9, Btn_Key0, Btn_KeyDot,
          Btn_KeyBackspace, Btn_KeyClear, Btn_KeyConfirm });

    ScreenButtonGroups.Add(EKioskScreen::BuySingle,
        { Btn_AddLocal, Btn_AddExpress, Btn_BuySingleContinue, Btn_ClearEntry });

    ScreenButtonGroups.Add(EKioskScreen::PayOption,
        { Btn_CreditDebit, Btn_Cash, Btn_SplitPayment });

    ScreenButtonGroups.Add(EKioskScreen::PayCard,   {});
    ScreenButtonGroups.Add(EKioskScreen::PayCash,   {});

    ScreenButtonGroups.Add(EKioskScreen::PayFinal,
        { Btn_PrintReceipt });

    ScreenButtonGroups.Add(EKioskScreen::ReadCard, {});

    ScreenButtonGroups.Add(EKioskScreen::DisplayInfo,
        { Btn_ReloadCard, Btn_ExitInfo });
}

void UOMNYCardComponent::SetScreen(EKioskScreen NewScreen)
{
    // Cancel any running timers when leaving a screen
    if (UWorld* W = GetWorld())
    {
        W->GetTimerManager().ClearTimer(PaymentTimerHandle);
        W->GetTimerManager().ClearTimer(ReadCardTimerHandle);
    }

    CurrentScreen = NewScreen;
    RefreshScreen(NewScreen);
    UpdateTextForScreen(NewScreen);

    // Auto-advance screens with no buttons
    if (UWorld* W = GetWorld())
    {
        if (NewScreen == EKioskScreen::PayCard || NewScreen == EKioskScreen::PayCash)
        {
            // 3-second simulated payment processing
            W->GetTimerManager().SetTimer(PaymentTimerHandle, this,
                &UOMNYCardComponent::OnPaymentProcessingDone, 3.f, false);
        }
        else if (NewScreen == EKioskScreen::ReadCard)
        {
            // 2-second simulated card tap — can also be triggered by TriggerCardTap()
            W->GetTimerManager().SetTimer(ReadCardTimerHandle, this,
                &UOMNYCardComponent::OnCardDetected, 2.f, false);
        }
    }
}

void UOMNYCardComponent::RefreshScreen(EKioskScreen Screen)
{
    // Swap background image
    UTexture2D* TexMap[] = {
        Tex_Home, Tex_AddBalance, Tex_AddCustom, Tex_BuySingle,
        Tex_PayOption, Tex_PayCard, Tex_PayCash, Tex_PayFinal,
        Tex_ReadCard, Tex_DisplayInfo
    };
    const int32 Idx = static_cast<int32>(Screen);
    if (TexMap[Idx]) SwapScreenTexture(TexMap[Idx]);

    // Hide every button, then show only those belonging to this screen
    for (auto& Pair : ScreenButtonGroups)
        for (UButton* Btn : Pair.Value)
            SetBtnVisible(Btn, false);

    if (auto* Group = ScreenButtonGroups.Find(Screen))
        for (UButton* Btn : *Group)
            SetBtnVisible(Btn, true);
}

void UOMNYCardComponent::SwapScreenTexture(UTexture2D* Tex)
{
    if (!ScreenImage || !Tex) return;
    FSlateBrush Brush;
    Brush.SetResourceObject(Tex);
    Brush.ImageSize = FVector2D(Tex->GetSizeX(), Tex->GetSizeY());
    ScreenImage->SetBrush(Brush);
}

void UOMNYCardComponent::SetBtnVisible(UButton* Btn, bool bVisible)
{
    if (!Btn) return;
    Btn->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UOMNYCardComponent::UpdateTextForScreen(EKioskScreen Screen)
{
    switch (Screen)
    {
    case EKioskScreen::AddBalance:
        if (Text_BalanceDisplay)
            Text_BalanceDisplay->SetText(FText::FromString(
                FString::Printf(TEXT("To add: $%.2f"), BalanceToAdd)));
        break;

    case EKioskScreen::AddCustom:
        if (Text_KeypadDisplay)
            Text_KeypadDisplay->SetText(FText::FromString(
                KeypadInput.IsEmpty() ? TEXT("$0.00") : TEXT("$") + KeypadInput));
        break;

    case EKioskScreen::BuySingle:
        if (Text_FareBreakdown)
            Text_FareBreakdown->SetText(FText::FromString(
                FString::Printf(TEXT("Local x%d  Express x%d  Total: $%.2f"),
                    LocalFareCount, ExpressFareCount, GetSessionTotal())));
        break;

    case EKioskScreen::PayOption:
        if (Text_BalanceDisplay)
            Text_BalanceDisplay->SetText(FText::FromString(
                FString::Printf(TEXT("Total: $%.2f"), GetSessionTotal())));
        break;

    case EKioskScreen::DisplayInfo:
        if (Text_CardBalance && ActiveCard)
            Text_CardBalance->SetText(FText::FromString(
                FString::Printf(TEXT("Balance: $%.2f"), ActiveCard->GetBalance())));
        break;

    default:
        break;
    }
}

float UOMNYCardComponent::GetSessionTotal() const
{
    if (BalanceFlow == EBalanceFlow::BuySingle)
        return LocalFareCount * LOCAL_FARE + ExpressFareCount * EXPRESS_FARE;
    return BalanceToAdd;
}

// ── Timer callbacks ───────────────────────────────────────────────────────────

void UOMNYCardComponent::OnPaymentProcessingDone()
{
    // Commit transaction to the card
    try
    {
        const float Total = GetSessionTotal();
        if (!ActiveCard)
        {
            ActiveCard = MakeShared<FOMNYCard>(TEXT("OMNY-NEW"), 0.f);
            CardInventory.Add(*ActiveCard);
        }
        ActiveCard->AddFunds(Total);
        TxLog.AddEntry(
            BalanceFlow == EBalanceFlow::BuySingle
                ? ETransactionType::FarePaid
                : ETransactionType::Reload,
            Total, ActiveCard->GetBalance());
        SaveProgress();
    }
    catch (const std::exception& Ex)
    {
        UE_LOG(LogTemp, Warning, TEXT("Payment error: %s"), UTF8_TO_TCHAR(Ex.what()));
    }

    SetScreen(EKioskScreen::PayFinal);
}

void UOMNYCardComponent::OnCardDetected()
{
    if (ReadCardContext == EReadCardContext::Reload)
    {
        BalanceFlow = EBalanceFlow::BuyNew;
        SetScreen(EKioskScreen::AddBalance);
    }
    else
    {
        SetScreen(EKioskScreen::DisplayInfo);
    }
}

void UOMNYCardComponent::TriggerCardTap()
{
    if (CurrentScreen != EKioskScreen::ReadCard) return;
    if (UWorld* W = GetWorld())
        W->GetTimerManager().ClearTimer(ReadCardTimerHandle);
    OnCardDetected();
}

// ── Home handlers ─────────────────────────────────────────────────────────────

void UOMNYCardComponent::OnBuyNew()
{
    BalanceToAdd = 0.f;
    BalanceFlow  = EBalanceFlow::BuyNew;
    SetScreen(EKioskScreen::AddBalance);
}

void UOMNYCardComponent::OnBuySingleNav()
{
    LocalFareCount = ExpressFareCount = 0;
    BalanceFlow = EBalanceFlow::BuySingle;
    SetScreen(EKioskScreen::BuySingle);
}

void UOMNYCardComponent::OnReload()
{
    ReadCardContext = EReadCardContext::Reload;
    SetScreen(EKioskScreen::ReadCard);
}

void UOMNYCardComponent::OnCheckInfo()
{
    ReadCardContext = EReadCardContext::CheckInfo;
    SetScreen(EKioskScreen::ReadCard);
}

// ── AddBalance handlers ───────────────────────────────────────────────────────

void UOMNYCardComponent::OnAdd10()
{
    try
    {
        if (BalanceToAdd + 10.f > MAX_LOAD)
            throw std::invalid_argument("Exceeds maximum load limit");
        BalanceToAdd += 10.f;
        UpdateTextForScreen(EKioskScreen::AddBalance);
    }
    catch (const std::exception& Ex)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s"), UTF8_TO_TCHAR(Ex.what()));
    }
}

void UOMNYCardComponent::OnAdd20()
{
    try
    {
        if (BalanceToAdd + 20.f > MAX_LOAD)
            throw std::invalid_argument("Exceeds maximum load limit");
        BalanceToAdd += 20.f;
        UpdateTextForScreen(EKioskScreen::AddBalance);
    }
    catch (const std::exception& Ex)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s"), UTF8_TO_TCHAR(Ex.what()));
    }
}

void UOMNYCardComponent::OnAddKeypad()
{
    KeypadInput.Empty();
    SetScreen(EKioskScreen::AddCustom);
}

void UOMNYCardComponent::OnAddFareCalc()
{
    LocalFareCount = ExpressFareCount = 0;
    BalanceFlow = EBalanceFlow::BuySingle;
    SetScreen(EKioskScreen::BuySingle);
}

void UOMNYCardComponent::OnAddBalanceContinue()
{
    if (BalanceToAdd <= 0.f)
    {
        UE_LOG(LogTemp, Warning, TEXT("No amount selected — add funds before continuing"));
        return;
    }
    SetScreen(EKioskScreen::PayOption);
}

// ── Keypad helpers ────────────────────────────────────────────────────────────

UOMNYCardComponent::FKeypadGrid UOMNYCardComponent::BuildKeypadGrid() const
{
    // Standard numeric keypad layout (4 rows × 3 cols)
    return {{
        {{ TEXT("1"), TEXT("2"), TEXT("3") }},
        {{ TEXT("4"), TEXT("5"), TEXT("6") }},
        {{ TEXT("7"), TEXT("8"), TEXT("9") }},
        {{ TEXT("."), TEXT("0"), TEXT("<") }}   // < = backspace
    }};
}

void UOMNYCardComponent::HandleKeypadInput(const FString& Key)
{
    // Enforce two decimal places and overall length
    if (Key == TEXT(".") && KeypadInput.Contains(TEXT("."))) return;

    if (KeypadInput.Contains(TEXT(".")))
    {
        const int32 DotIdx = KeypadInput.Find(TEXT("."));
        if (KeypadInput.Len() - DotIdx > 2) return;   // two decimal places max
    }

    if (KeypadInput.Len() >= 7) return;   // cap at $9999.99

    KeypadInput += Key;
    UpdateTextForScreen(EKioskScreen::AddCustom);
}

bool UOMNYCardComponent::ValidateKeypadAmount() const
{
    if (KeypadInput.IsEmpty()) return false;
    const float Val = FCString::Atof(*KeypadInput);
    return Val > 0.f && Val <= MAX_LOAD;
}

// ── BuySingle handlers ────────────────────────────────────────────────────────

void UOMNYCardComponent::OnAddLocal()
{
    ++LocalFareCount;
    UpdateTextForScreen(EKioskScreen::BuySingle);
}

void UOMNYCardComponent::OnAddExpress()
{
    ++ExpressFareCount;
    UpdateTextForScreen(EKioskScreen::BuySingle);
}

void UOMNYCardComponent::OnBuySingleContinue()
{
    if (LocalFareCount == 0 && ExpressFareCount == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No fares selected"));
        return;
    }
    SetScreen(EKioskScreen::PayOption);
}

void UOMNYCardComponent::OnClearEntry()
{
    LocalFareCount = ExpressFareCount = 0;
    UpdateTextForScreen(EKioskScreen::BuySingle);
}

// ── PayOption handlers ────────────────────────────────────────────────────────

void UOMNYCardComponent::OnCreditDebit()
{
    SelectedPayment = EPaymentMethod::CreditDebit;
    SetScreen(EKioskScreen::PayCard);
}

void UOMNYCardComponent::OnCash()
{
    SelectedPayment = EPaymentMethod::Cash;
    SetScreen(EKioskScreen::PayCash);
}

void UOMNYCardComponent::OnSplitPayment()
{
    SelectedPayment = EPaymentMethod::Split;
    SetScreen(EKioskScreen::PayCard);   // split starts with card portion
}

// ── PayFinal handler ──────────────────────────────────────────────────────────

void UOMNYCardComponent::OnPrintReceipt()
{
    // Reset session variables, return to Home
    BalanceToAdd     = 0.f;
    LocalFareCount   = 0;
    ExpressFareCount = 0;
    SelectedPayment  = EPaymentMethod::None;
    SetScreen(EKioskScreen::Home);
}

// ── DisplayInfo handlers ──────────────────────────────────────────────────────

void UOMNYCardComponent::OnReloadCard()
{
    BalanceToAdd = 0.f;
    BalanceFlow  = EBalanceFlow::BuyNew;
    SetScreen(EKioskScreen::AddBalance);
}

void UOMNYCardComponent::OnExitInfo()
{
    SetScreen(EKioskScreen::Home);
}

// ── Save / Load ───────────────────────────────────────────────────────────────

FString UOMNYCardComponent::GetSaveFilePath() const
{
    return FPaths::ProjectSavedDir() / TEXT("VROMNYProgress.txt");
}

void UOMNYCardComponent::SaveProgress() const
{
    const FString Path = GetSaveFilePath();
    FString Content;

    if (ActiveCard)
        Content += FString::Printf(TEXT("CardID=%s\nBalance=%.2f\n"),
            *ActiveCard->GetCardID(), ActiveCard->GetBalance());

    Content += FString::Printf(TEXT("Screen=%d\n"), static_cast<int32>(CurrentScreen));
    FFileHelper::SaveStringToFile(Content, *Path);

    // Also persist the transaction log alongside
    TxLog.SaveToFile(FPaths::ProjectSavedDir() / TEXT("VROMNYTransactions.txt"));
}

void UOMNYCardComponent::LoadProgress()
{
    const FString Path = GetSaveFilePath();
    FString Content;

    if (!FFileHelper::LoadFileToString(Content, *Path))
        return;   // no save file — start fresh

    try
    {
        TArray<FString> Lines;
        Content.ParseIntoArrayLines(Lines);
        FString CardID;
        float   Balance = 0.f;

        for (const FString& Line : Lines)
        {
            FString Key, Val;
            if (!Line.Split(TEXT("="), &Key, &Val)) continue;

            if (Key == TEXT("CardID"))   CardID  = Val;
            if (Key == TEXT("Balance"))  Balance = FCString::Atof(*Val);
        }

        if (!CardID.IsEmpty())
        {
            ActiveCard = MakeShared<FOMNYCard>(CardID, Balance);
            CardInventory.Add(*ActiveCard);
        }

        TxLog.LoadFromFile(FPaths::ProjectSavedDir() / TEXT("VROMNYTransactions.txt"));
    }
    catch (const std::exception& Ex)
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadProgress failed: %s"), UTF8_TO_TCHAR(Ex.what()));
    }
}
