#include "CaptureTheFlagGameMode.h"
#include "PirateWar/Type/Team.h"
#include "PirateWar/Weapon/Flag.h"
#include "PirateWar/Actor/FlagZone.h"
#include "PirateWar/GameState/MainGameState.h"

void ACaptureTheFlagGameMode::PlayerEliminated(APirateCharacter* ElimmedCharacter,
                                               APiratePlayerController* VictimController, APiratePlayerController* AttackerController)
{
	AMainGameMode::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);
}

void ACaptureTheFlagGameMode::FlagCaptured(AFlag* Flag, AFlagZone* Zone)
{
	bool bValidCapture = Flag->GetTeam() != Zone->Team;
	AMainGameState* BGameState = Cast<AMainGameState>(GameState);
	if (BGameState)
	{
		if (Zone->Team == ETeam::ET_BlueTeam)
		{
			BGameState->BlueTeamScores();
		}
		if (Zone->Team == ETeam::ET_RedTeam)
		{
			BGameState->RedTeamScores();
		}
	}
}
