#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PiratePlayerController.generated.h"

UCLASS()
class PIRATEWAR_API APiratePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);

protected:
	virtual void BeginPlay() override;

private:
	class APirateHUD* PirateHUD;
};
