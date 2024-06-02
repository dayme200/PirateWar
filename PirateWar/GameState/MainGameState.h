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

private:
	float TopScore = 0.f;
};
