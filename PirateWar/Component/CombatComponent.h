#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PirateWar/HUD/PirateHUD.h"
#include "PirateWar/Type/CombatState.h"
#include "PirateWar/Weapon/WeaponTypes.h"
#include "CombatComponent.generated.h"

class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIRATEWAR_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class APirateCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AWeapon* WeaponToEquip);
	void SwapWeapon();
	
	void Reload();

	UFUNCTION(BlueprintCallable)
	void FinishSwap();
	
	UFUNCTION(BlueprintCallable)
	void FinishSwapAttachWeapon();
	
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	
	void FireButtonPressed(bool bPressed);
	void ThrowGrenade();
	UFUNCTION(Server,Reliable)
	void ServerThrowGrenade();
	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();
	UFUNCTION(BlueprintCallable)
	void LaunchGrenade();
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> GrenadeClass;
	UFUNCTION(Server, Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize& Target);

	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);
	bool bLocallyReloading = false;
	
	FVector HitTarget;

	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();
	void JumpToShotgunEnd();
	
protected:
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget, float FireDelay);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void LocalPunch();
	UFUNCTION(Server, Reliable)
	void ServerPunch();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPunch();

	UPROPERTY(EditAnywhere)
	bool bCanPunch = true;
	UPROPERTY(EditAnywhere)
	float PunchCoolTime = .25f;
	void PunchCoolTimeFinished();
	FTimerHandle PunchTimerHandle;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	void ShotgunLocalFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();
	int32 AmountToReload();

	/*
	 * EquipWeapon
	 */
	void DropEquippedWeapon();
	void AttachActorToRightHand(AActor* ActorToAttach);
	void AttachActorToBack(AActor* ActorToAttach);
	void AttachActorToLeftHand(AActor* ActorToAttach);
	void AttachFlagToLeftHand(AWeapon* Flag);
	void UpdateCarriedAmmo();
	void PlayEquipWeaponSound(AWeapon* WeaponToEquip);
	void ReloadEmptyWeapon();
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);
	
	void ShowAttachedGrenade(bool bShowGrenade);
	
private:
	UPROPERTY()
	class APirateCharacter* Character;
	UPROPERTY()
	class APiratePlayerController* Controller;
	UPROPERTY()
	class APirateHUD* HUD;
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;
	UFUNCTION()
	void OnRep_EquippedWeapon();
	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	AWeapon* SecondaryWeapon;
	UFUNCTION()
	void OnRep_SecondaryWeapon();
	
	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming;

	bool bAimingButtonPressed = false;
	
	UFUNCTION()
	void OnRep_Aiming();

	bool bFireButtonPressed;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	/*
	 * HUD and crosshair
	 */
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShottingFactor;

	FHUDPackage HUDPackage;

	/*
	 * Aiming and FOV
	 * Field of view when not aiming; set to the camera's base FOV in BeginPlay
	 */
	float DefaultFOV;
	float CurrentFOV;

	/*
	 * Automatic fire
	 */
	FTimerHandle FireTimer;
	bool bCanFire = true;
	void StartFireTimer();
	void FireTimerFinished();
	void Fire();
	void FireProjectileWeapon();
	void FireHitScanWeapon();
	void FireShotgunWeapon();
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> Shake;

	bool CanFire();

	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;
	UFUNCTION()
	void OnRep_CarriedAmmo();
	TMap<EWeaponType, int32> CarriedAmmoMap;
	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 500;
	void InitializeCarriedAmmo();
	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;
	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 8;
	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 8;
	UPROPERTY(EditAnywhere)
	int32 StartingSMGAmmo = 45;
	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo = 8;
	UPROPERTY(EditAnywhere)
	int32 StartingSniperRifleAmmo = 8;
	UPROPERTY(EditAnywhere)
	int32 StartingGrenadeLauncherAmmo = 8;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;
	void InterpFOV(float DeltaTime);

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unonccupired;
	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();
	void UpdateShotgunAmmoValues();

	UPROPERTY(ReplicatedUsing = OnRep_Grenade)
	int32 Grenade = 3;
	UFUNCTION()
	void OnRep_Grenade();
	UPROPERTY(EditAnywhere)
	int32 MaxGrenade = 3;
	void UpdateHUDGrenade();

	UPROPERTY(ReplicatedUsing = OnRep_HoldingTheFlag)
	bool bHoldingTheFlag = false;
	UFUNCTION()
	void OnRep_HoldingTheFlag();

	UPROPERTY()
	AWeapon* TheFlag;
	
public:
	FORCEINLINE int32 GetGrenade() const { return Grenade; }
	bool ShouldSwapWeapon();
};
