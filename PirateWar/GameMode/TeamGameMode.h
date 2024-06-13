#pragma once

#include "CoreMinimal.h"
#include "MainGameMode.h"
#include "TeamGameMode.generated.h"

UCLASS()
class PIRATEWAR_API ATeamGameMode : public AMainGameMode
{
	GENERATED_BODY()
	
public:
	ATeamGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage) override;
	virtual void PlayerEliminated(class APirateCharacter* ElimmedCharacter,
		class APiratePlayerController* VictimController,
		APiratePlayerController* AttackerController
	) override;
	
	bool bTeamsMatch = false;
	
protected:
	virtual void HandleMatchHasStarted() override;
};
