#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MainGameState.generated.h"

class APiratePlayerState;

UCLASS()
class PIRATEWAR_API AMainGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(APiratePlayerState* ScoringPlayer);

	UPROPERTY(Replicated)
	TArray<APiratePlayerState*> TopScoringPlayers;

	/** 
	* Teams
	*/
	void RedTeamScores();
	void BlueTeamScores();

	TArray<APiratePlayerState*> RedTeam;
	TArray<APiratePlayerState*> BlueTeam;

	UPROPERTY(ReplicatedUsing = OnRep_RedTeamScore)
	float RedTeamScore = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_BlueTeamScore)
	float BlueTeamScore = 0.f;

	UFUNCTION()
	void OnRep_RedTeamScore();

	UFUNCTION()
	void OnRep_BlueTeamScore();
	
private:
	float TopScore = 0.f;
};
