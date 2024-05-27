#include "PirateCharacter.h"

APirateCharacter::APirateCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

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
}

