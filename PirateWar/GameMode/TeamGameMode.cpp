#include "TeamGameMode.h"
#include "PirateWar/Type/Team.h"
#include "Kismet/GameplayStatics.h"
#include "PirateWar/GameState/MainGameState.h"
#include "PirateWar/PlayerState/PiratePlayerState.h"
#include "PirateWar/PlayerController/PiratePlayerController.h"

ATeamGameMode::ATeamGameMode()
{
	bTeamsMatch = true;
}

void ATeamGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	AMainGameState* BGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		APiratePlayerState* BPState = NewPlayer->GetPlayerState<APiratePlayerState>();
		if (BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
		{
			if (BGameState->BlueTeam.Num() >= BGameState->RedTeam.Num())
			{
				BGameState->RedTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_RedTeam);
			}
			else
			{
				BGameState->BlueTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_BlueTeam);
			}
		}
	}
}

void ATeamGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	AMainGameState* BGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(this));
	APiratePlayerState* BPState = Exiting->GetPlayerState<APiratePlayerState>();
	if (BGameState && BPState)
	{
		if (BGameState->RedTeam.Contains(BPState))
		{
			BGameState->RedTeam.Remove(BPState);
		}
		if (BGameState->BlueTeam.Contains(BPState))
		{
			BGameState->BlueTeam.Remove(BPState);
		}
	}

}

float ATeamGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	APiratePlayerState* AttackerPState = Attacker->GetPlayerState<APiratePlayerState>();
	APiratePlayerState* VictimPState = Victim->GetPlayerState<APiratePlayerState>();
	if (AttackerPState == nullptr || VictimPState == nullptr) return BaseDamage;
	if (VictimPState == AttackerPState)
	{
		return BaseDamage;
	}
	if (AttackerPState->GetTeam() == VictimPState->GetTeam())
	{
		return 0.f;
	}
	return BaseDamage;
}

void ATeamGameMode::PlayerEliminated(APirateCharacter* ElimmedCharacter, APiratePlayerController* VictimController,
	APiratePlayerController* AttackerController)
{
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);

	AMainGameState* BGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(this));
	APiratePlayerState* AttackerPlayerState = AttackerController ? Cast<APiratePlayerState>(AttackerController->PlayerState) : nullptr;
	if (BGameState && AttackerPlayerState)
	{
		if (AttackerPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			BGameState->BlueTeamScores();
		}
		if (AttackerPlayerState->GetTeam() == ETeam::ET_RedTeam)
		{
			BGameState->RedTeamScores();
		}
	}
}


void ATeamGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	
	AMainGameState* BGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		for (auto PState : BGameState->PlayerArray)
		{
			APiratePlayerState* BPState = Cast<APiratePlayerState>(PState.Get());
			if (BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (BGameState->BlueTeam.Num() >= BGameState->RedTeam.Num())
				{
					BGameState->RedTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					BGameState->BlueTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}
}
