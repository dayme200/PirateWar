#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PirateWar/Type/Team.h"
#include "PiratePlayerState.generated.h"

UCLASS()
class PIRATEWAR_API APiratePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Score() override;
	UFUNCTION()
	virtual void OnRep_Defeat();
	void AddToScore(float ScoreAmount);
	void AddToDefeat(int32 DefeatAmount);

private:
	UPROPERTY()
	class APirateCharacter* Character;
	UPROPERTY()
	class APiratePlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeat)
	int32 Defeat;
	
	UPROPERTY(ReplicatedUsing = OnRep_Team)
	ETeam Team = ETeam::ET_NoTeam;

	UFUNCTION()
	void OnRep_Team();

public:
	FORCEINLINE ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam TeamToSet);
};
