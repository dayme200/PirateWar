#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial Stae"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_MAX UMETA(DisplayName = "Default Max"),
};

UCLASS()
class PIRATEWAR_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	void ShowPickupWidget(bool bShowWidget);

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnSphereoverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

private:
	UPROPERTY(VisibleAnywhere, Category = WeaponProperties)
	USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, Category = WeaponProperties)
	class USphereComponent* AreaSphere;
	UPROPERTY(VisibleAnywhere, Category = WeaponProperties)
	EWeaponState WeaponState;
	UPROPERTY(VisibleAnywhere, Category = WeaponProperties)
	class UWidgetComponent* PickupWidget;
};
