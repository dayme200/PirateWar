#include "Shotgun.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"
#include "PirateWar/Character/PirateCharacter.h"
#include "PirateWar/Component/LagCompensationComponent.h"
#include "PirateWar/PlayerController/PiratePlayerController.h"

void AShotgun::Fire(const FVector& HitTarget)
{
	AWeapon::Fire(FVector());
	
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();
	
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation() + SocketTransform.GetRotation().GetForwardVector() * 25.f;
		
		TMap<APirateCharacter*, uint32> HitMap;
		for (uint32 i = 0; i < NumberOfPellets; i++)
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
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					HitSound,
					FireHit.ImpactPoint,
					.5f,
					FMath::FRandRange(-.5f, .5f)
				);
			}
		}

		TArray<APirateCharacter*> HitCharacters;
		
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key && InstigatorController)
			{
				bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthDamage)
				{
					UGameplayStatics::ApplyDamage(
						HitPair.Key,
						Damage * HitPair.Value,
						InstigatorController,
						this,
						UDamageType::StaticClass()
					);
				}

				HitCharacters.Add(HitPair.Key);
			}
		}
		
		// if (!HasAuthority() && bUseServerSideRewind)
		// {
		// 	PirateOwnerCharacter = PirateOwnerCharacter == nullptr ? Cast<APirateCharacter>(GetOwner()) : PirateOwnerCharacter;
		// 	PirateOwnerController = PirateOwnerController == nullptr ? Cast<APiratePlayerController>(InstigatorController) : PirateOwnerController;
		// 	if (PirateOwnerCharacter && PirateOwnerController && PirateOwnerCharacter->GetLagCompensation() && PirateOwnerCharacter->IsLocallyControlled())
		// 	{
		// 		PirateOwnerCharacter->GetLagCompensation()->ShotgunServerScoreRequest(
		// 			HitCharacters,
		// 			Start,
		// 			HitTargets,
		// 			PirateOwnerController->GetServerTime() - PirateOwnerController->SingleTripTime
		// 		);
		// 	}
		// }
	}
}

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector Start = SocketTransform.GetLocation();

		// Maps hit character to number of times hit
		TMap<APirateCharacter*, uint32> HitMap;
		TMap<APirateCharacter*, uint32> HeadShotHitMap;
		for (FVector_NetQuantize HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			APirateCharacter* PirateCharacter = Cast<APirateCharacter>(FireHit.GetActor());
			if (PirateCharacter)
			{
				const bool bHeadShot = FireHit.BoneName.ToString() == FString("head");

				if (bHeadShot)
				{
					if (HeadShotHitMap.Contains(PirateCharacter)) HeadShotHitMap[PirateCharacter]++;
					else HeadShotHitMap.Emplace(PirateCharacter, 1);
				}
				else
				{
					if (HitMap.Contains(PirateCharacter)) HitMap[PirateCharacter]++;
					else HitMap.Emplace(PirateCharacter, 1);
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
				if (HitSound)
				{
					UGameplayStatics::PlaySoundAtLocation(
						this,
						HitSound,
						FireHit.ImpactPoint,
						.5f,
						FMath::FRandRange(-.5f, .5f)
					);
				}
			}
		}
		TArray<APirateCharacter*> HitCharacters;

		// Maps Character hit to total damage
		TMap<APirateCharacter*, float> DamageMap;

		// Calculate body shot damage by multiplying times hit x Damage - store in DamageMap
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key)
			{
				DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);

				HitCharacters.AddUnique(HitPair.Key);
			}
		}

		// Calculate head shot damage by multiplying times hit x HeadShotDamage - store in DamageMap
		for (auto HeadShotHitPair : HeadShotHitMap)
		{
			if (HeadShotHitPair.Key)
			{
				if (DamageMap.Contains(HeadShotHitPair.Key)) DamageMap[HeadShotHitPair.Key] += HeadShotHitPair.Value * HeadShotDamage;
				else DamageMap.Emplace(HeadShotHitPair.Key, HeadShotHitPair.Value * HeadShotDamage);

				HitCharacters.AddUnique(HeadShotHitPair.Key);
			}
		}

		// Loop through DamageMap to get total damage for each character
		for (auto DamagePair : DamageMap)
		{
			if (DamagePair.Key && InstigatorController)
			{
				bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthDamage)
				{
					UGameplayStatics::ApplyDamage(
						DamagePair.Key, // Character that was hit
						DamagePair.Value, // Damage calculated in the two for loops above
						InstigatorController,
						this,
						UDamageType::StaticClass()
					);
				}
			}
		}


		if (!HasAuthority() && bUseServerSideRewind)
		{
			PirateOwnerCharacter = PirateOwnerCharacter == nullptr ? Cast<APirateCharacter>(OwnerPawn) : PirateOwnerCharacter;
			PirateOwnerController = PirateOwnerController == nullptr ? Cast<APiratePlayerController>(InstigatorController) : PirateOwnerController;
			if (PirateOwnerController && PirateOwnerCharacter && PirateOwnerCharacter->GetLagCompensation() && PirateOwnerCharacter->IsLocallyControlled())
			{
				PirateOwnerCharacter->GetLagCompensation()->ShotgunServerScoreRequest(
					HitCharacters,
					Start,
					HitTargets,
					PirateOwnerController->GetServerTime() - PirateOwnerController->SingleTripTime
				);
			}
		}
	}
}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation() + SocketTransform.GetRotation().GetForwardVector() * 25.f;
	
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	
	for (uint32 i = 0; i < NumberOfPellets; i++)
	{
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLoc = SphereCenter + RandVec;
		FVector ToEndLoc = EndLoc - TraceStart;
		ToEndLoc = TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size();
		
		HitTargets.Add(ToEndLoc);
	}
}
