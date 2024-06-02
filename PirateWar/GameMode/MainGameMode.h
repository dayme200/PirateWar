#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainGameMode.generated.h"

UCLASS()
class PIRATEWAR_API AMainGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AMainGameMode();
	virtual void PlayerEliminated(
		class APirateCharacter* ElimmedCharacter,
		class APiratePlayerController* VictimController,
		APiratePlayerController* AttackerController
	);
	virtual void RequestRespawn(class APirateCharacter* ElimmedCharacter, AController* ElimmedController);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 5.f;
	float LevelStartingTime = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnMatchStateSet() override;
	
private:
	float CountDownTime = 0.f;
};