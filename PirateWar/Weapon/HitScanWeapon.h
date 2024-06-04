#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

UCLASS()
class PIRATEWAR_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;

protected:
	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticle;
	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticle;
	UPROPERTY(EditAnywhere)
	USoundCue* Hitsound;
	
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;
	
private:
	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere)
	USoundCue* FireSound;

	/*
	 * Trace end with Scatter
	 */
	UPROPERTY(EditAnywhere, Category = Scatter)
	float DistanceToSphere = 800.f;
	UPROPERTY(EditAnywhere, Category = Scatter)
	float SphereRadius = 75.f;
	UPROPERTY(EditAnywhere, Category = Scatter)
	bool bUseScatter = false;
};
