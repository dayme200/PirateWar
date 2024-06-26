#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "RocketProjectile.generated.h"

UCLASS()
class PIRATEWAR_API ARocketProjectile : public AProjectile
{
	GENERATED_BODY()

public:
	ARocketProjectile();
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;
	virtual void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

	UPROPERTY(EditAnywhere)
	USoundCue* ProjectileLoop;
	UPROPERTY()
	UAudioComponent* ProjectileLoopComponent;
	UPROPERTY(EditAnywhere)
	USoundAttenuation* LoopingSoundAttenuation;

	UPROPERTY(VisibleAnywhere)
	class URocketMovementComponent* RocketMovementComponent;
	
private:

};
