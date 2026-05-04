#include "KioskWidget.h"

void UKioskWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Btn_BuyNew)      Btn_BuyNew->OnClicked.AddDynamic(this, &UKioskWidget::OnBuyNew);
    if (Btn_BuySingle)   Btn_BuySingle->OnClicked.AddDynamic(this, &UKioskWidget::OnBuySingle);
    if (Btn_Reload)      Btn_Reload->OnClicked.AddDynamic(this, &UKioskWidget::OnReload);
    if (Btn_CheckInfo)   Btn_CheckInfo->OnClicked.AddDynamic(this, &UKioskWidget::OnCheckInfo);

    if (Btn_Add10)       Btn_Add10->OnClicked.AddDynamic(this, &UKioskWidget::OnAdd10);
    if (Btn_Add20)       Btn_Add20->OnClicked.AddDynamic(this, &UKioskWidget::OnAdd20);
    if (Btn_AddKeypad)   Btn_AddKeypad->OnClicked.AddDynamic(this, &UKioskWidget::OnAddKeypadBtn);
    if (Btn_AddFareCalc) Btn_AddFareCalc->OnClicked.AddDynamic(this, &UKioskWidget::OnAddFareCalc);

    if (Btn_CreditDebit)  Btn_CreditDebit->OnClicked.AddDynamic(this, &UKioskWidget::OnCreditDebit);
    if (Btn_Cash)         Btn_Cash->OnClicked.AddDynamic(this, &UKioskWidget::OnCash);
    if (Btn_SplitPayment) Btn_SplitPayment->OnClicked.AddDynamic(this, &UKioskWidget::OnSplitPayment);

    if (Btn_PrintReceipt) Btn_PrintReceipt->OnClicked.AddDynamic(this, &UKioskWidget::OnPrintReceipt);

    if (Btn_AddLocal)    Btn_AddLocal->OnClicked.AddDynamic(this, &UKioskWidget::OnAddLocal);
    if (Btn_AddExpress)  Btn_AddExpress->OnClicked.AddDynamic(this, &UKioskWidget::OnAddExpress);
    if (Btn_Continue)    Btn_Continue->OnClicked.AddDynamic(this, &UKioskWidget::OnBuySingleContinue);
    if (Btn_ClearEntry)  Btn_ClearEntry->OnClicked.AddDynamic(this, &UKioskWidget::OnClearEntry);

    if (Btn_ReloadCard)  Btn_ReloadCard->OnClicked.AddDynamic(this, &UKioskWidget::OnReloadCard);
    if (Btn_ExitInfo)    Btn_ExitInfo->OnClicked.AddDynamic(this, &UKioskWidget::OnExitInfo);

    GoToScreen(EKioskScreen::Home);
}

// ── Navigation ────────────────────────────────────────────────────────────────

void UKioskWidget::GoToScreen(EKioskScreen Screen)
{
    CurrentScreen = Screen;
    ApplyScreenTexture(Screen);
    RefreshButtonStates();
    OnScreenChanged(Screen);

    if (CardComponent)
        OnBalanceUpdated(CardComponent->GetBalance());
}

// ── Panel visibility ──────────────────────────────────────────────────────────

void UKioskWidget::ShowOnlyPanel(UPanelWidget* Active)
{
    if (WS_Screen && Active)
        WS_Screen->SetActiveWidget(Active);
}

// ── Button state management ───────────────────────────────────────────────────

void UKioskWidget::RefreshButtonStates()
{
    // All buttons that exist in the widget.
    TArray<UButton*> All = {
        Btn_BuyNew, Btn_BuySingle, Btn_Reload, Btn_CheckInfo,
        Btn_Add10, Btn_Add20, Btn_AddKeypad, Btn_AddFareCalc,
        Btn_CreditDebit, Btn_Cash, Btn_SplitPayment,
        Btn_PrintReceipt,
        Btn_AddLocal, Btn_AddExpress, Btn_Continue, Btn_ClearEntry,
        Btn_ReloadCard, Btn_ExitInfo
    };

    // Disable everything first — then selectively re-enable.
    for (UButton* B : All)
        if (B) B->SetIsEnabled(false);

    switch (CurrentScreen)
    {
    case EKioskScreen::Home:
        if (Btn_BuyNew)    Btn_BuyNew->SetIsEnabled(true);
        if (Btn_BuySingle) Btn_BuySingle->SetIsEnabled(true);
        if (Btn_Reload)    Btn_Reload->SetIsEnabled(true);
        if (Btn_CheckInfo) Btn_CheckInfo->SetIsEnabled(true);
        ShowOnlyPanel(HomePanel);
        break;

    case EKioskScreen::AddBalance:
        if (Btn_Add10)       Btn_Add10->SetIsEnabled(true);
        if (Btn_Add20)       Btn_Add20->SetIsEnabled(true);
        if (Btn_AddKeypad)   Btn_AddKeypad->SetIsEnabled(true);
        if (Btn_AddFareCalc) Btn_AddFareCalc->SetIsEnabled(true);
        ShowOnlyPanel(AddBalancePanel);
        break;

    case EKioskScreen::PayOption:
        if (Btn_CreditDebit)  Btn_CreditDebit->SetIsEnabled(true);
        if (Btn_Cash)         Btn_Cash->SetIsEnabled(true);
        if (Btn_SplitPayment) Btn_SplitPayment->SetIsEnabled(true);
        ShowOnlyPanel(PayOptionPanel);
        break;

    case EKioskScreen::AddKeypad:
        ShowOnlyPanel(AddKeypadPanel);
        break;

    case EKioskScreen::PayCard:
        ShowOnlyPanel(PayCardPanel);
        break;

    case EKioskScreen::PayCash:
        ShowOnlyPanel(PayCashPanel);
        break;

    case EKioskScreen::ReadCard:
        ShowOnlyPanel(ReadCardPanel);
        break;

    case EKioskScreen::PayFinal:
        if (Btn_PrintReceipt) Btn_PrintReceipt->SetIsEnabled(true);
        ShowOnlyPanel(PayFinalPanel);
        break;

    case EKioskScreen::BuySingle:
        if (Btn_AddLocal)  Btn_AddLocal->SetIsEnabled(true);
        if (Btn_AddExpress)Btn_AddExpress->SetIsEnabled(true);
        if (Btn_Continue)  Btn_Continue->SetIsEnabled(true);
        if (Btn_ClearEntry)Btn_ClearEntry->SetIsEnabled(true);
        ShowOnlyPanel(BuySinglePanel);
        break;

    case EKioskScreen::DisplayInfo:
        if (Btn_ReloadCard) Btn_ReloadCard->SetIsEnabled(true);
        if (Btn_ExitInfo)   Btn_ExitInfo->SetIsEnabled(true);
        ShowOnlyPanel(DisplayInfoPanel);
        break;
    }
}

void UKioskWidget::ApplyScreenTexture(EKioskScreen Screen)
{
    if (!Img_Screen) return;
    if (UTexture2D** Tex = ScreenTextures.Find(Screen))
        Img_Screen->SetBrushFromTexture(*Tex, true);
}

// ── Home ──────────────────────────────────────────────────────────────────────

void UKioskWidget::OnBuyNew()
{
    CurrentFlow = EKioskFlow::BuyNew;
    if (CardComponent)
        CardComponent->InitCard(TEXT("Player"), 0.f);
    GoToScreen(EKioskScreen::AddBalance);
}

void UKioskWidget::OnBuySingle()
{
    CurrentFlow      = EKioskFlow::BuySingle;
    BuySingleTotal   = 0.f;
    LocalRideCount   = 0;
    ExpressRideCount = 0;
    GoToScreen(EKioskScreen::BuySingle);
}

void UKioskWidget::OnReload()
{
    CurrentFlow = EKioskFlow::Reload;
    GoToScreen(EKioskScreen::ReadCard);
}

void UKioskWidget::OnCheckInfo()
{
    CurrentFlow = EKioskFlow::CheckInfo;
    GoToScreen(EKioskScreen::ReadCard);
}

// ── AddBalance ────────────────────────────────────────────────────────────────

void UKioskWidget::OnAdd10()
{
    PendingAmount = 10.f;
    OnTotalUpdated(PendingAmount);
    GoToScreen(EKioskScreen::PayOption);
}

void UKioskWidget::OnAdd20()
{
    PendingAmount = 20.f;
    OnTotalUpdated(PendingAmount);
    GoToScreen(EKioskScreen::PayOption);
}

void UKioskWidget::OnAddKeypad(float Amount)
{
    if (!ValidateAmount(Amount))
    {
        OnError(TEXT("Please enter an amount between $0.01 and $100.00."));
        return;
    }
    PendingAmount = Amount;
    OnTotalUpdated(PendingAmount);
    GoToScreen(EKioskScreen::PayOption);
}

void UKioskWidget::OnAddKeypadBtn()
{
    GoToScreen(EKioskScreen::AddKeypad);
}

void UKioskWidget::OnAddFareCalc()
{
    CurrentFlow      = EKioskFlow::BuySingle;
    BuySingleTotal   = 0.f;
    LocalRideCount   = 0;
    ExpressRideCount = 0;
    GoToScreen(EKioskScreen::BuySingle);
}

// ── PayOption ─────────────────────────────────────────────────────────────────

void UKioskWidget::OnCreditDebit()  { GoToScreen(EKioskScreen::PayCard); }
void UKioskWidget::OnCash()         { GoToScreen(EKioskScreen::PayCash); }
void UKioskWidget::OnSplitPayment() { GoToScreen(EKioskScreen::PayCard); }

// ── Payment processing ────────────────────────────────────────────────────────

void UKioskWidget::OnPaymentProcessed()
{
    CommitFunds();
    GoToScreen(EKioskScreen::PayFinal);
}

// ── PayFinal ──────────────────────────────────────────────────────────────────

void UKioskWidget::OnPrintReceipt()
{
    GoToScreen(EKioskScreen::Home);
}

// ── BuySingle ─────────────────────────────────────────────────────────────────

void UKioskWidget::OnAddLocal()
{
    ++LocalRideCount;
    BuySingleTotal += LocalFare;
    OnTotalUpdated(BuySingleTotal);
}

void UKioskWidget::OnAddExpress()
{
    ++ExpressRideCount;
    BuySingleTotal += ExpressFare;
    OnTotalUpdated(BuySingleTotal);
}

void UKioskWidget::OnBuySingleContinue()
{
    if (BuySingleTotal <= 0.f)
    {
        OnError(TEXT("Please add at least one ride before continuing."));
        return;
    }
    PendingAmount = BuySingleTotal;
    GoToScreen(EKioskScreen::PayOption);
}

void UKioskWidget::OnClearEntry()
{
    BuySingleTotal   = 0.f;
    LocalRideCount   = 0;
    ExpressRideCount = 0;
    OnTotalUpdated(0.f);
}

// ── ReadCard ──────────────────────────────────────────────────────────────────

void UKioskWidget::SimulateCardRead()
{
    if (!CardComponent || !CardComponent->IsValid())
    {
        OnError(TEXT("Card read failed. Please tap your card again."));
        return;
    }

    if      (CurrentFlow == EKioskFlow::Reload)    GoToScreen(EKioskScreen::AddBalance);
    else if (CurrentFlow == EKioskFlow::CheckInfo) GoToScreen(EKioskScreen::DisplayInfo);
}

// ── DisplayInfo ───────────────────────────────────────────────────────────────

void UKioskWidget::OnReloadCard()
{
    CurrentFlow = EKioskFlow::Reload;
    GoToScreen(EKioskScreen::AddBalance);
}

void UKioskWidget::OnExitInfo()
{
    GoToScreen(EKioskScreen::Home);
}

// ── Helpers ───────────────────────────────────────────────────────────────────

bool UKioskWidget::ValidateAmount(float Amount) const
{
    return Amount >= UOMNYCardComponent::MinAdd && Amount <= UOMNYCardComponent::MaxBalance;
}

void UKioskWidget::CommitFunds()
{
    if (CurrentFlow == EKioskFlow::BuySingle)
    {
        // Single-ride ticket — no card balance involved.
        OnBalanceUpdated(CardComponent ? CardComponent->GetBalance() : 0.f);
        PendingAmount = 0.f;
        return;
    }

    if (!CardComponent) return;

    if (!CardComponent->AddFunds(PendingAmount))
        OnError(TEXT("Could not add funds. Amount may exceed the $100 card limit."));
    else
        OnBalanceUpdated(CardComponent->GetBalance());

    PendingAmount = 0.f;
}
