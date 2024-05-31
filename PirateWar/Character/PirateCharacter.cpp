#include "PirateCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "PirateWar/Weapon/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PirateWar/Component/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

APirateCharacter::APirateCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);
	
	Combat2 = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat2"));
	Combat2->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	TurningInPlace = ETurningInPlace::ETP_NotTurning;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}

void APirateCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat2)
	{
		Combat2->Character = this;
	}
}

void APirateCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat2 == nullptr || Combat2->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void APirateCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APirateCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);
}

void APirateCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APirateCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APirateCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APirateCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &APirateCharacter::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APirateCharacter::Jump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APirateCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APirateCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APirateCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &APirateCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &APirateCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &APirateCharacter::EquipButtonPressed);
}

void APirateCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APirateCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void APirateCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void APirateCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void APirateCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void APirateCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void APirateCharacter::CrouchButtonPressed()
{
	if (bIsCrouched) UnCrouch();
	else Crouch();
}

void APirateCharacter::FireButtonPressed()
{
	if (Combat2)
	{
		Combat2->FireButtonPressed(true);
	}
}

void APirateCharacter::FireButtonReleased()
{
	if (Combat2)
	{
		Combat2->FireButtonPressed(false);
	}
}

void APirateCharacter::AimButtonPressed()
{
	if (Combat2)
	{
		Combat2->SetAiming(true);
	}
}

void APirateCharacter::AimButtonReleased()
{
	if (Combat2)
	{
		Combat2->SetAiming(false);
	}
}

void APirateCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void APirateCharacter::AimOffset(float DeltaTime)
{
	if (Combat2 && Combat2->EquippedWeapon == nullptr) return;
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) // standing still, not jumping
	{
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) // running, or jumping
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETP_NotTurning;
	}
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		FVector2d InRange(270.f, 360.f);
		FVector2d OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void APirateCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void APirateCharacter::EquipButtonPressed()
{
	if (Combat2)
	{
		if (!HasAuthority())
		{
			Combat2->EquipWeapon(OverlappingWeapon);
		}
		ServerEquipButtonPressed();
	}
}

void APirateCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat2)
	{
		Combat2->EquipWeapon(OverlappingWeapon);
	}
}

void APirateCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void APirateCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon =	Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool APirateCharacter::IsWeaponEquipped()
{
	return (Combat2 && Combat2->EquippedWeapon);
}

bool APirateCharacter::IsAiming()
{
	return (Combat2 && Combat2->bAiming);
}

AWeapon* APirateCharacter::GetEquippedWeapon()
{
	if (Combat2 == nullptr) return nullptr;
	return Combat2->EquippedWeapon;
}
