#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "PirateWar/Weapon/WeaponTypes.h"
#include "AmmoPickup.generated.h"

UCLASS()
class PIRATEWAR_API AAmmoPickup : public APickup
{
	GENERATED_BODY()
	
protected:
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

private:
	UPROPERTY(EditAnywhere)
	int32 AmmoAmount = 30.f;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
};
