#include "PiratePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "PirateWar/Character/PirateCharacter.h"
#include "PirateWar/PlayerController/PiratePlayerController.h"

void APiratePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APiratePlayerState, Defeat);
	DOREPLIFETIME(APiratePlayerState, Team);
}

void APiratePlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<APirateCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<APiratePlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void APiratePlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? Cast<APirateCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<APiratePlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(ScoreAmount);
		}
	}
}

void APiratePlayerState::AddToDefeat(int32 DefeatAmount)
{
	Defeat += DefeatAmount;
	Character = Character == nullptr ? Cast<APirateCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<APiratePlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeat(Defeat);
		}
	}
}

void APiratePlayerState::OnRep_Team()
{
	APirateCharacter* BCharacter = Cast <APirateCharacter>(GetPawn());
	if (BCharacter)
	{
		BCharacter->SetTeamColor(Team);
	}
}

void APiratePlayerState::SetTeam(ETeam TeamToSet)
{
	Team = TeamToSet;

	APirateCharacter* BCharacter = Cast <APirateCharacter>(GetPawn());
	if (BCharacter)
	{
		BCharacter->SetTeamColor(Team);
	}
}

void APiratePlayerState::OnRep_Defeat()
{
	Character = Character == nullptr ? Cast<APirateCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<APiratePlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeat(Defeat);
		}
	}
}
