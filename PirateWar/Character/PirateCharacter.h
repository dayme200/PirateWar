#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Type/TurningInPlace.h"
#include "PirateWar/Interface/InteractWithCrosshairInterface.h"
#include "PirateCharacter.generated.h"

UCLASS()
class PIRATEWAR_API APirateCharacter : public ACharacter, public IInteractWithCrosshairInterface
{
	GENERATED_BODY()

public:
	APirateCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	void PlayFireMontage(bool bAiming);
	void PlayHitReactMontage();
	void PlayElimMontage();

	virtual void OnRep_ReplicatedMovement() override;
	
protected:
	virtual void BeginPlay() override;

	/*
	 * Input Mapping
	 */
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void FireButtonPressed();
	void FireButtonReleased();
	void AimButtonPressed();
	void AimButtonReleased();
	virtual void Jump() override;

	void SimProxiesTurn();
	void CalculateAo_Pitch();
	void AimOffset(float DeltaTime);
	UFUNCTION()
	void ReceiveDamage(
		AActor* DamagedActor,
		float Damage,
		const UDamageType* DamageType,
		class AController* InstigatorController,
		AActor* DamageCauser
	);
	void UpdateHUDHealth();
	void UpdateHUDScore();
	void PollInit();
	
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat2;
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ElimMontage;

	void HideCameraIfCharacterClose();
	float CameraThreshold = 200.f;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	/*
	 * Player Health
	 */
	UPROPERTY(EditAnywhere, Category = Stat)
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	float Health = 100.f;
	UFUNCTION()
	void OnRep_Health();

	class APiratePlayerController* PiratePlayerController;

	/*
	 * Elim
	 */
	bool bElimmed = false;
	FTimerHandle ElimTimer;
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;
	void ElimTimerFinished();

	class APiratePlayerState* PiratePlayerState;
	
public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
};
