#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PirateCharacter.generated.h"

UCLASS()
class PIRATEWAR_API APirateCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APirateCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

private:	

};
