#include "ProjectileWeapon.h"
#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);


	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	UWorld* World = GetWorld();
	if (MuzzleFlashSocket && World)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		// From muzzle flash socket to hit location from TraceUnderCrosshairs
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotator = ToTarget.Rotation();
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;

		AProjectile* SpawnedProjectile = nullptr;
		if (bUseServerSideRewind)
		{
			if (InstigatorPawn->HasAuthority())
			{
				if (InstigatorPawn->IsLocallyControlled())
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(
						ProjectileClass,
						SocketTransform.GetLocation(),
						TargetRotator,
						SpawnParams
					);
					SpawnedProjectile->bUseServerSideRewind = false;
					SpawnedProjectile->Damage = Damage;
					SpawnedProjectile->HeadShotDamage = HeadShotDamage;
				}
				else
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(
						ServerSideRewindProjectileClass,
						SocketTransform.GetLocation(),
						TargetRotator,
						SpawnParams
					);
					SpawnedProjectile->bUseServerSideRewind = true;
				}
			}
			else
			{
				if (InstigatorPawn->IsLocallyControlled())
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(
						ServerSideRewindProjectileClass,
						SocketTransform.GetLocation(),
						TargetRotator,
						SpawnParams
					);
					SpawnedProjectile->bUseServerSideRewind = true;
					SpawnedProjectile->TraceStart = SocketTransform.GetLocation();
					SpawnedProjectile->InitialVelocity = SpawnedProjectile->GetActorForwardVector() * SpawnedProjectile->InitialSpeed;
				}
				else
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(
						ServerSideRewindProjectileClass,
						SocketTransform.GetLocation(),
						TargetRotator,
						SpawnParams
					);
					SpawnedProjectile->bUseServerSideRewind = false;
				}
			}
		}
		else
		{
			if (InstigatorPawn->HasAuthority())
			{
				SpawnedProjectile = World->SpawnActor<AProjectile>(
						ProjectileClass,
						SocketTransform.GetLocation(),
						TargetRotator,
						SpawnParams
					);
				SpawnedProjectile->bUseServerSideRewind = false;
				SpawnedProjectile->Damage = Damage;
				SpawnedProjectile->HeadShotDamage = HeadShotDamage;
			}
		}
		
		
	}
}
