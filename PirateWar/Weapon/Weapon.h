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

class UTexture2D;

UCLASS()
class PIRATEWAR_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector& HitTarget);
	void Dropped();

	UPROPERTY(EditAnywhere, Category = WeaponProperties)
	class UAnimationAsset* FireAnimation;

	/*
	 * Textures for the weapon crosshairs
	 */
	UPROPERTY(EditAnywhere)
	UTexture2D* CrosshairCenter;
	UPROPERTY(EditAnywhere)
	UTexture2D* CrosshairLeft;
	UPROPERTY(EditAnywhere)
	UTexture2D* CrosshairRight;
	UPROPERTY(EditAnywhere)
	UTexture2D* CrosshairTop;
	UPROPERTY(EditAnywhere)
	UTexture2D* CrosshairBottom;

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
	class UWidgetComponent* PickupWidget;
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = WeaponProperties)
	EWeaponState WeaponState;
	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;

	/*
	 * Zoomed FOV while aiming
	 */
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;
	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;
	
public:
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }

	/*
	 * Automatic Fire
	 */
	UPROPERTY(EditAnywhere)
	float FireDelay = .15f;
	UPROPERTY(EditAnywhere)
	bool bAutomatic = true;
};
