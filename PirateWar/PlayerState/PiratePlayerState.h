#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PiratePlayerState.generated.h"

UCLASS()
class PIRATEWAR_API APiratePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void OnRep_Score() override;
	void AddToScore(float ScoreAmount);

private:
	class APirateCharacter* Character;
	class APiratePlayerController* Controller;
};
