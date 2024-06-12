#include "BulletProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PirateWar/Character/PirateCharacter.h"
#include "PirateWar/Component/LagCompensationComponent.h"
#include "PirateWar/PlayerController/PiratePlayerController.h"

ABulletProjectile::ABulletProjectile()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = InitialSpeed;
}

#if WITH_EDITOR
void ABulletProjectile::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);
	// FName PropertyName = Event.Property() != nullptr ? Event.Property->GetFName() : NAME_None;
	// if (PropertyName == GET_MEMBER_NAME_CHECKED(ABulletProjectile, InitialSpeed))
	// {
	// 	if (ProjectileMovementComponent)
	// 	{
	// 		ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	// 		ProjectileMovementComponent->MaxSpeed = InitialSpeed;
	// 	}
	// }
}
#endif

void ABulletProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	APirateCharacter* OwnerCharacter = Cast<APirateCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		APiratePlayerController* OwnerController = Cast<APiratePlayerController>(OwnerCharacter->Controller);
		if (OwnerController)
		{
			if (OwnerCharacter->HasAuthority() && !bUseServerSideRewind)
			{
				UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
				Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
				return;
			}
			APirateCharacter* HitCharacter = Cast<APirateCharacter>(OtherActor);
			if (bUseServerSideRewind && OwnerCharacter->GetLagCompensation() && OwnerCharacter->IsLocallyControlled() && HitCharacter)
			{
				OwnerCharacter->GetLagCompensation()->ProjectileServerScoreRequest(
					HitCharacter,
					TraceStart,
					InitialVelocity,
					OwnerController->GetServerTime() - OwnerController->SingleTripTime
				);
			}
		}
	}
	
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void ABulletProjectile::BeginPlay()
{
	Super::BeginPlay();

	// FPredictProjectilePathParams PathParams;
	// PathParams.bTraceWithChannel = true;
	// PathParams.bTraceWithCollision = true;
	// PathParams.DrawDebugTime = 5.f;
	// PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	// PathParams.LaunchVelocity = GetActorForwardVector() * InitialSpeed;
	// PathParams.MaxSimTime = 4.f;
	// PathParams.ProjectileRadius = 5.f;
	// PathParams.SimFrequency = 30.f;
	// PathParams.StartLocation = GetActorLocation();
	// PathParams.TraceChannel = ECC_Visibility;
	// PathParams.ActorsToIgnore.Add(this);
	// FPredictProjectilePathResult PathResult;
	// UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);
}
