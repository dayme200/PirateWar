#include "HealthPickup.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PirateWar/Component/BuffComponent.h"
#include "PirateWar/Character/PirateCharacter.h"

AHealthPickup::AHealthPickup()
{
	bReplicates = true;

	PickupEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PickupEffectComponent"));
	PickupEffectComponent->SetupAttachment(RootComponent);
}

void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	APirateCharacter* PirateCharacter = Cast<APirateCharacter>(OtherActor);
	if (PirateCharacter)
	{
		UBuffComponent* Buff = Cast<UBuffComponent>(PirateCharacter->GetBuff());
		if (Buff)
		{
			Buff->Heal(HealAmount, HealingTime);
		}
	}
	Destroy();
}

void AHealthPickup::Destroyed()
{
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			PickupEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}
	Super::Destroyed();
}
