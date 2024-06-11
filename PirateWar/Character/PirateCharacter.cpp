#include "PirateCharacter.h"
#include "Net/UnrealNetwork.h"
#include "PirateWar/PirateWar.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PirateWar/Weapon/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "PirateWar/GameMode/MainGameMode.h"
#include "GameFramework/SpringArmComponent.h"
#include "PirateWar/Component/BuffComponent.h"
#include "PirateWar/Component/CombatComponent.h"
#include "PirateWar/PlayerState/PiratePlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PirateWar/Component/LagCompensationComponent.h"
#include "PirateWar/PlayerController/PiratePlayerController.h"

APirateCharacter::APirateCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
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

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("Buff"));
	Buff->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	TurningInPlace = ETurningInPlace::ETP_NotTurning;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttachedGrenade"));
	AttachedGrenade->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/*
	 *	Hit boxes for server-side rewind
	 */
	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), FName("head"));
	head->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	pelvis->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	spine_01 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_01"));
	spine_01->SetupAttachment(GetMesh(), FName("spine_01"));
	spine_01->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	spine_02->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	spine_03->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	spine_04 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_04"));
	spine_04->SetupAttachment(GetMesh(), FName("spine_04"));
	spine_04->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	spine_05 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_05"));
	spine_05->SetupAttachment(GetMesh(), FName("spine_05"));
	spine_05->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	upperarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	upperarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	lowerarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	lowerarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	hand_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	hand_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	thigh_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	thigh_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	calf_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	calf_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	foot_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	foot_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APirateCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat2)
	{
		Combat2->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		Buff->SetInitialSpeed(GetCharacterMovement()->GetMaxSpeed(), GetCharacterMovement()->MaxWalkSpeedCrouched);
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<APiratePlayerController>(Controller);
		}
	}
}

void APirateCharacter::SpawnDefaultWeapon()
{
	AMainGameMode* MainGameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this));
	UWorld* World = GetWorld();
	if (MainGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (Combat2)
		{
			Combat2->EquipWeapon(StartingWeapon);
		}
	}
}

void APirateCharacter::BeginPlay()
{
	Super::BeginPlay();
	SpawnDefaultWeapon();
	UpdateHUDAmmo();
	UpdateHUDHealth();
	UpdateHUDShield();
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &APirateCharacter::ReceiveDamage);
	}
	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}
}

void APirateCharacter::Destroyed()
{
	Super::Destroyed();

	AMainGameMode* MainGameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = MainGameMode && MainGameMode->GetMatchState() != MatchState::InProgress;
	if (Combat2 && Combat2->EquippedWeapon && bMatchNotInProgress)
	{
		Combat2->EquippedWeapon->Destroy();
	}
}

void APirateCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateInPlace(DeltaTime);
	HideCameraIfCharacterClose();
	PollInit();
}

void APirateCharacter::Elim()
{
	DropOrDestroyWeapons();
	MulticastElim();
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&APirateCharacter::ElimTimerFinished,
		ElimDelay
	);
}

void APirateCharacter::DropOrDestroyWaepon(AWeapon* Weapon)
{
	if (Weapon == nullptr) return;
	if (Weapon->bDestroyWeapon)
	{
		Weapon->Destroy();
	}
	else
	{
		Weapon->Dropped();
	}
}

void APirateCharacter::DropOrDestroyWeapons()
{
	if (Combat2)
	{
		if (Combat2->EquippedWeapon)
		{
			DropOrDestroyWaepon(Combat2->EquippedWeapon);
		}
		if (Combat2->SecondaryWeapon)
		{
			DropOrDestroyWaepon(Combat2->SecondaryWeapon);
		}
	}
}

void APirateCharacter::MulticastElim_Implementation()
{
	if (PiratePlayerController)
	{
		PiratePlayerController->SetHUDWeaponAmmo(0);
	}
	bElimmed = true;
	PlayElimMontage();

	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	bDisableGameplay = true;
	GetCharacterMovement()->DisableMovement();
	if (Combat2)
	{
		Combat2->FireButtonPressed(false);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (IsLocallyControlled() && Combat2 && Combat2->bAiming && Combat2->EquippedWeapon && Combat2->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRife)
	{
		ShowSniperScopeWidget(false);
	}
}

void APirateCharacter::ElimTimerFinished()
{
	AMainGameMode* MainGameMode = GetWorld()->GetAuthGameMode<AMainGameMode>();
	if (MainGameMode)
	{
		MainGameMode->RequestRespawn(this, Controller);
	}
}

void APirateCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;	
}

void APirateCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETP_NotTurning;
		return;
	}
	if (GetLocalRole() > ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAo_Pitch();
	}
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
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APirateCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &APirateCharacter::GrenadeButtonPressed);
}

void APirateCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APirateCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(APirateCharacter, Health);
	DOREPLIFETIME(APirateCharacter, Shield);
	DOREPLIFETIME(APirateCharacter, bDisableGameplay);
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

	void APirateCharacter::PlayReloadMontage()
{
	if (Combat2 == nullptr || Combat2->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);	
		FName SectionName;
		switch (GetEquippedWeapon()->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRife:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("RocketLauncher");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("SubmachineGun");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SniperRife:
			SectionName = FName("SniperRifle");
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("GrenadeLauncher");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void APirateCharacter::PlayHitReactMontage()
{
	if (Combat2 == nullptr || Combat2->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);	
		//TODO Change Section
		FName SectionName = FName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void APirateCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void APirateCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

void APirateCharacter::MoveForward(float Value)
{
	if (bDisableGameplay) return;
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void APirateCharacter::MoveRight(float Value)
{
	if (bDisableGameplay) return;
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
	if (bDisableGameplay) return;
	if (bIsCrouched) UnCrouch();
	else Crouch();
}

void APirateCharacter::FireButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat2)
	{
		Combat2->FireButtonPressed(true);
	}
}

void APirateCharacter::FireButtonReleased()
{
	if (bDisableGameplay) return;
	if (Combat2)
	{
		Combat2->FireButtonPressed(false);
	}
}

void APirateCharacter::AimButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat2)
	{
		Combat2->SetAiming(true);
	}
}

void APirateCharacter::AimButtonReleased()
{
	if (bDisableGameplay) return;
	if (Combat2)
	{
		Combat2->SetAiming(false);
	}
}

void APirateCharacter::ReloadButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat2)
	{
		Combat2->Reload();
	}
}

void APirateCharacter::GrenadeButtonPressed()
{
	if (Combat2)
	{
		Combat2->ThrowGrenade();
	}
}

void APirateCharacter::Jump()
{
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void APirateCharacter::SimProxiesTurn()
{
	if (Combat2 == nullptr || Combat2->EquippedWeapon == nullptr) return;
	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETP_NotTurning;
		return;
	}
	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETP_NotTurning;
}

void APirateCharacter::CalculateAo_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		FVector2d InRange(270.f, 360.f);
		FVector2d OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void APirateCharacter::AimOffset(float DeltaTime)
{
	if (Combat2 && Combat2->EquippedWeapon == nullptr) return;
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) // standing still, not jumping
	{
		bRotateRootBone = true;
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
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETP_NotTurning;
	}
	CalculateAo_Pitch();
}

void APirateCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	if (bElimmed) return;

	float DamageToHealth = Damage;
	if (Shield > 0.f)
	{
		if (Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield - Damage, 0.f, MaxShield);
			DamageToHealth = 0.f;
		}
		else
		{
			Shield = 0.f;
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f, Damage);
		}
	}
	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);
	UpdateHUDHealth();
	UpdateHUDShield();
	PlayHitReactMontage();

	if (Health == 0.f)
	{
		AMainGameMode* MainGameMode = GetWorld()->GetAuthGameMode<AMainGameMode>();
		if (MainGameMode)
		{
			PiratePlayerController = PiratePlayerController == nullptr ? Cast<APiratePlayerController>(Controller) : PiratePlayerController;
			APiratePlayerController* AttackerController = Cast<APiratePlayerController>(InstigatorController);
			MainGameMode->PlayerEliminated(this, PiratePlayerController, AttackerController);
		}
	}
}

void APirateCharacter::UpdateHUDHealth()
{
	PiratePlayerController = PiratePlayerController == nullptr ? Cast<APiratePlayerController>(Controller) : PiratePlayerController;
	if (PiratePlayerController)
	{
		PiratePlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void APirateCharacter::UpdateHUDShield()
{
	PiratePlayerController = PiratePlayerController == nullptr ? Cast<APiratePlayerController>(Controller) : PiratePlayerController;
	if (PiratePlayerController)
	{
		PiratePlayerController->SetHUDShield(Shield, MaxShield);
	}
}

void APirateCharacter::UpdateHUDAmmo()
{
	PiratePlayerController = PiratePlayerController == nullptr ? Cast<APiratePlayerController>(Controller) : PiratePlayerController;
	if (PiratePlayerController && Combat2 && Combat2->EquippedWeapon)
	{
		PiratePlayerController->SetHUDCarriedAmmo(Combat2->CarriedAmmo);
		PiratePlayerController->SetHUDWeaponAmmo(Combat2->EquippedWeapon->GetAmmo());
	}
}

void APirateCharacter::PollInit()
{
	if (PiratePlayerState == nullptr)
	{
		PiratePlayerState = GetPlayerState<APiratePlayerState>();
		if (PiratePlayerState)
		{
			PiratePlayerState->AddToScore(0.f);
			PiratePlayerState->AddToDefeat(0);
		}
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
	if (bDisableGameplay) return;
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
		if (OverlappingWeapon)
		{
			Combat2->EquipWeapon(OverlappingWeapon);
		}
		else if (Combat2->bShouldSwapWeapon())
		{
			Combat2->SwapWeapon();
		}
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

void APirateCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat2 && Combat2->EquippedWeapon && Combat2->EquippedWeapon->GetWeaponMesh())
		{
			Combat2->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat2 && Combat2->EquippedWeapon && Combat2->EquippedWeapon->GetWeaponMesh())
		{
			Combat2->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

float APirateCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void APirateCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (Health < LastHealth)
	{
		PlayHitReactMontage();
	}
}

void APirateCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
	if (Shield < LastShield)
	{
		PlayHitReactMontage();
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

FVector APirateCharacter::GetHitTarget() const
{
	if (Combat2 == nullptr) return FVector();
	return Combat2->HitTarget;
}

ECombatState APirateCharacter::GetCombatState() const
{
	if (Combat2 == nullptr) return ECombatState::ECS_MAX;
	return Combat2->CombatState;
}

bool APirateCharacter::IsLocallyReloading()
{
	if (Combat2 == nullptr) return false;
	return Combat2->bLocallyReloading;
}
