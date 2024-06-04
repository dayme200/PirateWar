#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "GrenadeProjectile.generated.h"

UCLASS()
class PIRATEWAR_API AGrenadeProjectile : public AProjectile
{
	GENERATED_BODY()

public:
	AGrenadeProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);
	virtual void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	) override;

private:
	UPROPERTY(EditAnywhere)
	USoundCue* BounceSound;
};
