#include "Shotgun.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"
#include "PirateWar/Character/PirateCharacter.h"

void AShotgun::Fire(const FVector& HitTarget)
{
	AWeapon::Fire(HitTarget);
	
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();
	
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket && InstigatorController)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation() + SocketTransform.GetRotation().GetForwardVector() * 50.f;
		uint32 Hits = 0;

		TMap<APirateCharacter*, uint32> HitMap;
		for (uint32 i = 0; i< NumberOfPellets; i++)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			APirateCharacter* PirateCharacter = Cast<APirateCharacter>(FireHit.GetActor());
			if (PirateCharacter && HasAuthority() && InstigatorController)
			{
				if (HitMap.Contains(PirateCharacter))
				{
					HitMap[PirateCharacter]++;
				}
				else
				{
					HitMap.Emplace(PirateCharacter, 1);
				}
			}
			if (ImpactParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticle,
					FireHit.ImpactPoint,
					FireHit.ImpactNormal.Rotation()
				);
			}
			if (Hitsound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					Hitsound,
					FireHit.ImpactPoint,
					.5f,
					FMath::FRandRange(-.5f, .5f)
				);
			}
		}

		for (auto HirPair : HitMap)
		{
			if (HirPair.Key && HasAuthority() && InstigatorController)
			{
				UGameplayStatics::ApplyDamage(
		HirPair.Key,
					Damage * HirPair.Value,
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
			}
		}
	}
}
