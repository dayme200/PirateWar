#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "BulletProjectile.generated.h"

UCLASS()
class PIRATEWAR_API ABulletProjectile : public AProjectile
{
	GENERATED_BODY()

protected:
	virtual void OnHit(UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	) override;
};
