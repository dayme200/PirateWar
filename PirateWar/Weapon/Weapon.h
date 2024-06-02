#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PirateWar/Weapon/WeaponTypes.h"
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
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector& HitTarget);
	void Dropped();
	void AddAmmo(int32 AddToAmmo);

	UPROPERTY(EditAnywhere, Category = WeaponProperties)
	class UAnimationAsset* FireAnimation;

	/*
	 * Ammo
	 */
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo = 30.f;
	UFUNCTION()
	void OnRep_Ammo();
	void SpendRound();
	UPROPERTY(EditAnywhere)
	int32 MagCapacity = 30.f;

	UPROPERTY()
	class APirateCharacter* PirateOwnerCharacter;
	UPROPERTY()
	class APiratePlayerController* PirateOwnerController;
	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
	
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
	bool IsEmpty();
	UPROPERTY(EditAnywhere)
	class USoundBase* EquipSound;

	
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
};
