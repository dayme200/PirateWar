#include "JumpPickup.h"
#include "PirateWar/Component/BuffComponent.h"
#include "PirateWar/Character/PirateCharacter.h"

void AJumpPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	APirateCharacter* PirateCharacter = Cast<APirateCharacter>(OtherActor);
	if (PirateCharacter)
	{
		UBuffComponent* Buff = Cast<UBuffComponent>(PirateCharacter->GetBuff());
		if (Buff)
		{
			Buff->BuffJump(JumpZVelocityBuff, JumpBuffTime);
		}
	}
	Destroy();
}
