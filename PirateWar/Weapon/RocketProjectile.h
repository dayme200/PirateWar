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

protected:
	virtual void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);
	
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;
};
