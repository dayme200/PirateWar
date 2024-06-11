#include "HitScanWeapon.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"
#include "PirateWar/Character/PirateCharacter.h"
#include "PirateWar/Component/LagCompensationComponent.h"
#include "PirateWar/PlayerController/PiratePlayerController.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();
	
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket && InstigatorController)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation() + SocketTransform.GetRotation().GetForwardVector() * 25.f;
		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);
		APirateCharacter* PirateCharacter = Cast<APirateCharacter>(FireHit.GetActor());
		if (PirateCharacter && InstigatorController && PirateCharacter != Cast<APirateCharacter>(PirateCharacter))
		{
			if (HasAuthority() && !bUseServerSideRewind)
			{
				UGameplayStatics::ApplyDamage(
		PirateCharacter,
					Damage,
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
			}
			else if (!HasAuthority() &&		bUseServerSideRewind)
			{
				PirateOwnerCharacter = PirateOwnerCharacter == nullptr ? Cast<APirateCharacter>(GetOwner()) : PirateOwnerCharacter;
				PirateOwnerController = PirateOwnerController == nullptr ? Cast<APiratePlayerController>(InstigatorController) : PirateOwnerController;
				if (PirateOwnerCharacter && PirateOwnerController && PirateOwnerCharacter->GetLagCompensation())
				{
					PirateOwnerCharacter->GetLagCompensation()->ServerScoreRequest(
						PirateCharacter,
						Start,
						HitTarget,
						PirateOwnerController->GetServerTime() - PirateOwnerController->SingleTripTime,
						this
					);
				}
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
				FireHit.ImpactPoint
			);
		}

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				MuzzleFlash,
				SocketTransform
			);
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				FireSound,
				GetActorLocation()
			);
		}
	}
}
void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector End = bUseScatter ? TraceEndWithScatter(HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;

		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Visibility
		);
		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}
		else
		{
			OutHit.ImpactPoint = End;
		}
		if (BeamParticle)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				World,
				BeamParticle,
				TraceStart,
				FRotator::ZeroRotator,
				true
			);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}
