#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainGameMode.generated.h"

namespace MatchState
{
	extern PIRATEWAR_API const FName Cooldown; 
}

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
	void PlayerLeftGame(class APiratePlayerState* PlayerLeaving);
	
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 5.f;
	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 5.f;
	float LevelStartingTime = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnMatchStateSet() override;
	
private:
	float CountDownTime = 0.f;

public:
	FORCEINLINE float GetCountDownTime() const { return CountDownTime; }
};