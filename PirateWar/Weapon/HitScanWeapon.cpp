#include "HitScanWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "PirateWar/Character/PirateCharacter.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();
	
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket && InstigatorController)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform((GetWeaponMesh()));
		FVector Start = SocketTransform.GetLocation();
		FVector End = Start + (HitTarget - Start) * 1.25;

		FHitResult FireHit;
		UWorld* World = GetWorld();
		if (World)
		{
			World->LineTraceSingleByChannel(
				FireHit,
				Start,
				End,
				ECC_Visibility
			);
			if (FireHit.bBlockingHit)
			{
				APirateCharacter* PirateCharacter = Cast<APirateCharacter>(FireHit.GetActor());
				if (PirateCharacter)
				{
					if (HasAuthority())
					{
						UGameplayStatics::ApplyDamage(
						PirateCharacter,
						Damage,
						InstigatorController,
						this,
						UDamageType::StaticClass()
					);
					}
				}
				if (ImpactParticle)
				{
					UGameplayStatics::SpawnEmitterAtLocation(
						World,
						ImpactParticle,
						FireHit.ImpactPoint,
						FireHit.ImpactNormal.Rotation()
					);
				}
			}
		}
	}
}
