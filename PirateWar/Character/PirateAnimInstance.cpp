#include "PirateAnimInstance.h"
#include "PirateCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPirateAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PirateCharacter = Cast<APirateCharacter>(TryGetPawnOwner());
}

void UPirateAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (PirateCharacter == nullptr) Cast<APirateCharacter>(TryGetPawnOwner());
	if (PirateCharacter == nullptr) return;

	FVector Velocity = PirateCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();
	bIsInAir = PirateCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = PirateCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
}
