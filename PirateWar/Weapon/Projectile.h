#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class PIRATEWAR_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;
	
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticle;
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;
	
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem;
	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent;
	void SpawnTrailSystem();

	void StartDestroyTimer();
	void DestroyTimerFinished();
	void ExplodeDamage();

	UPROPERTY(EditAnywhere)
	float DamageInnerRadius = 200.f;
	UPROPERTY(EditAnywhere)
	float DamageOuterRadius = 500.f;
	
private:
	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;
	UPROPERTY()
	class UParticleSystemComponent* TracerComponent;

	FTimerHandle DestroyHandle;
	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;
};
