#include "RocketProjectile.h"
#include "Sound/SoundCue.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "RocketMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"

ARocketProjectile::ARocketProjectile()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("RocketMovementComponent"));
	RocketMovementComponent->bRotationFollowsVelocity = true;
	RocketMovementComponent->SetIsReplicated(true);
	RocketMovementComponent->ProjectileGravityScale = 0.f;
	RocketMovementComponent->InitialSpeed = 1500.f;
	RocketMovementComponent->MaxSpeed = 1500.f;
}

void ARocketProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ARocketProjectile::OnHit);
	}
	
	SpawnTrailSystem();
	
	if (ProjectileLoop && LoopingSoundAttenuation)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(
			ProjectileLoop,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			EAttachLocation::KeepWorldPosition,
			false,
			1.f,
			1.f,
			0.f,
			LoopingSoundAttenuation,
			(USoundConcurrency*)nullptr,
			false
		);
	}
}

void ARocketProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == Owner)
	{
		return;
	}
	ExplodeDamage();
	StartDestroyTimer();

	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ImpactParticle,
			GetActorTransform()
		);
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ImpactSound,
			GetActorLocation()
		);
	}
	if (ProjectileMesh)
	{
		ProjectileMesh->SetVisibility(false);
	}
	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (TrailSystemComponent && TrailSystemComponent->IsActive())
	{
		TrailSystemComponent->Deactivate();
	}
	if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}
}

void ARocketProjectile::Destroyed()
{
	Super::Destroyed();
}
