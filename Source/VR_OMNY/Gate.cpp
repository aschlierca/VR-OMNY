#include "Gate.h"
#include "VR_OMNYCharacter.h"
#include "Components/BoxComponent.h"

AGate::AGate()
{
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AGate::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AGate::OnOverlapEnd);
}

void AGate::BeginPlay()
{
	Super::BeginPlay();
}

void AGate::TryOpen(AVR_OMNYCharacter *Player)
{
	if (!Player)
		return;

	bool bSuccess = Player->TapCardAtGate(2.90f);
	bIsOpen = bSuccess;
	OnGateTapped.Broadcast(bSuccess);
	// Blueprint handles: green/red material flash, chime/buzz SFX, gate mesh animation.
}

void AGate::OnOverlapBegin(UPrimitiveComponent *, AActor *OtherActor,
						   UPrimitiveComponent *, int32, bool, const FHitResult &)
{
	if (AVR_OMNYCharacter *Player = Cast<AVR_OMNYCharacter>(OtherActor))
	{
		Player->bNearGate  = true;
		Player->NearbyGate = this;
		// Show "Press F to tap card" HUD prompt from Blueprint.
	}
}

void AGate::OnOverlapEnd(UPrimitiveComponent *, AActor *OtherActor,
						 UPrimitiveComponent *, int32)
{
	if (AVR_OMNYCharacter *Player = Cast<AVR_OMNYCharacter>(OtherActor))
	{
		Player->bNearGate  = false;
		Player->NearbyGate = nullptr;
	}
}
