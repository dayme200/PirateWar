#pragma once

#include "CoreMinimal.h"
#include "TeamGameMode.h"
#include "CaptureTheFlagGameMode.generated.h"

UCLASS()
class PIRATEWAR_API ACaptureTheFlagGameMode : public ATeamGameMode
{
	GENERATED_BODY()
	
public:
	virtual void PlayerEliminated(class APirateCharacter* ElimmedCharacter,
		class APiratePlayerController* VictimController,
		APiratePlayerController* AttackerController
	) override;
	void FlagCaptured(class AFlag* Flag, class AFlagZone* Zone);
};
