#include "PirateAnimInstance.h"
#include "PirateCharacter.h"
#include "PirateWar/Weapon/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPirateAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PirateCharacter = Cast<APirateCharacter>(TryGetPawnOwner());
}

void UPirateAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (PirateCharacter == nullptr) Cast<APirateCharacter>(TryGetPawnOwner());
	if (PirateCharacter == nullptr) return;

	FVector Velocity = PirateCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();
	bIsInAir = PirateCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = PirateCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = PirateCharacter->IsWeaponEquipped();
	EquippedWeapon = PirateCharacter->GetEquippedWeapon();
	bIsCrouched = PirateCharacter->bIsCrouched;
	bIsAiming = PirateCharacter->IsAiming();
	TurningInPlace = PirateCharacter->GetTurningInPlace();
	bRotateRootBone = PirateCharacter->ShouldRotateRootBone();
	bElimmed = PirateCharacter->IsElimmed();

	// Offset Yaw for Strafing
	FRotator AimRotation = PirateCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(PirateCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;
	
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = PirateCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	AO_Yaw = PirateCharacter->GetAO_Yaw();
	AO_Pitch = PirateCharacter->GetAO_Pitch();

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && PirateCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		PirateCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));
		
		// FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), RTS_World);
		// FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
		// DrawDebugLine(GetWorld(),MuzzleTipTransform.GetLocation(),MuzzleTipTransform.GetLocation()+MuzzleX*1000.f,FColor::Red);
		// DrawDebugLine(GetWorld(),MuzzleTipTransform.GetLocation(),PirateCharacter->GetHitTarget(),FColor::Blue);

		if (PirateCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PirateCharacter->GetHitTarget(), RightHandTransform.GetLocation());
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.f);
		}
	}

	bUseFABRIK = PirateCharacter->GetCombatState() == ECombatState::ECS_Unonccupired;
	if (PirateCharacter->IsLocallyControlled() && PirateCharacter->GetCombatState() != ECombatState::ECS_ThrowingGrenade)
	{
		bUseFABRIK = !PirateCharacter->IsLocallyReloading();
	}
	bUseOffsets = PirateCharacter->GetCombatState() == ECombatState::ECS_Unonccupired && !PirateCharacter->GetDisableGameplay();
	bTransformRightHand = PirateCharacter->GetCombatState() == ECombatState::ECS_Unonccupired && !PirateCharacter->GetDisableGameplay();
}
