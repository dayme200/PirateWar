#include "PirateCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "PirateWar/Weapon/Weapon.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PirateWar/Component/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

APirateCharacter::APirateCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
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
}

void APirateCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat2)
	{
		Combat2->Character = this;
	}
}

void APirateCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APirateCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APirateCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APirateCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APirateCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APirateCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &APirateCharacter::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APirateCharacter::CrouchButtonPressed);
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

void APirateCharacter::EquipButtonPressed()
{
	if (Combat2)
	{
		if (HasAuthority())
		{
			Combat2->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
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
