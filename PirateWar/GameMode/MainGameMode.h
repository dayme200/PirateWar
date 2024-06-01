#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainGameMode.generated.h"

UCLASS()
class PIRATEWAR_API AMainGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void PlayerEliminated(
		class APirateCharacter* ElimmedCharacter,
		class APiratePlayerController* VictimController,
		APiratePlayerController* AttackerController
	);
	virtual void RequestRespawn(class APirateCharacter* ElimmedCharacter, AController* ElimmedController);

protected:
	virtual void BeginPlay() override;
};