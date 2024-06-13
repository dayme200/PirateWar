#include "MainGameState.h"
#include "Net/UnrealNetwork.h"
#include "PirateWar/PlayerState/PiratePlayerState.h"
#include "PirateWar/PlayerController/PiratePlayerController.h"

void AMainGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMainGameState, TopScoringPlayers);
	DOREPLIFETIME(AMainGameState, RedTeamScore);
	DOREPLIFETIME(AMainGameState, BlueTeamScore);
}

void AMainGameState::UpdateTopScore(APiratePlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

void AMainGameState::RedTeamScores()
{
	++RedTeamScore;
	APiratePlayerController* BPlayer = Cast<APiratePlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDRedTeamScore(RedTeamScore);
	}
}

void AMainGameState::BlueTeamScores()
{
	++BlueTeamScore;
	APiratePlayerController* BPlayer = Cast<APiratePlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDBlueTeamScore(BlueTeamScore);
	}
}

void AMainGameState::OnRep_RedTeamScore()
{
	APiratePlayerController* BPlayer = Cast<APiratePlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDRedTeamScore(RedTeamScore);
	}
}

void AMainGameState::OnRep_BlueTeamScore()
{
	APiratePlayerController* BPlayer = Cast<APiratePlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDBlueTeamScore(BlueTeamScore);
	}
}
