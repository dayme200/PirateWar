#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "PirateWar/Type/Team.h"
#include "TeamPlayerStart.generated.h"

UCLASS()
class PIRATEWAR_API ATeamPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	ETeam Team;
};
