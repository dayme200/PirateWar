#include "PiratePlayerState.h"
#include "PirateWar/Character/PirateCharacter.h"
#include "PirateWar/PlayerController/PiratePlayerController.h"

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
