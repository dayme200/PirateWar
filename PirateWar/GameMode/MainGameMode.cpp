#include "MainGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "PirateWar/GameState/MainGameState.h"
#include "PirateWar/Character/PirateCharacter.h"
#include "PirateWar/PlayerState/PiratePlayerState.h"
#include "PirateWar/PlayerController/PiratePlayerController.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldonw");
}

AMainGameMode::AMainGameMode()
{
	bDelayedStart = true;
	bUseSeamlessTravel = true;
}

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void AMainGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetMatchState() == MatchState::WaitingToStart)
	{
		CountDownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountDownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountDownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CooldownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void AMainGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APiratePlayerController* PiratePlayerController = Cast<APiratePlayerController>(*It);
		if (PiratePlayerController)
		{
			PiratePlayerController->OnMatchStateSet(MatchState);
		}
	}
}

void AMainGameMode::PlayerEliminated(APirateCharacter* ElimmedCharacter, APiratePlayerController* VictimController,
                                     APiratePlayerController* AttackerController)
{
	APiratePlayerState* AttackerPlayerState = AttackerController ? Cast<APiratePlayerState>(AttackerController->PlayerState) : nullptr;
	APiratePlayerState* VictimPlayerState = VictimController ? Cast<APiratePlayerState>(VictimController->PlayerState) : nullptr;
	AMainGameState* MainGameState = GetGameState<AMainGameState>();
	
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && MainGameState)
	{
		TArray<APiratePlayerState*> PlayersCurrentlyInTheLead;
		for (auto LeadPlayer : MainGameState->TopScoringPlayers)
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);
		}
		AttackerPlayerState->AddToScore(1.f);
		MainGameState->UpdateTopScore(AttackerPlayerState);
		if (MainGameState->TopScoringPlayers.Contains(AttackerPlayerState))
		{
			APirateCharacter* Leader = Cast<APirateCharacter>(AttackerPlayerState->GetPawn());
			if (Leader)
			{
				Leader->MulticastGainedTheLead();
			}
		}

		for (int32 i = 0; i < PlayersCurrentlyInTheLead.Num(); i++)
		{
			if (!MainGameState->TopScoringPlayers.Contains(PlayersCurrentlyInTheLead[i]))
			{
				APirateCharacter* Loser = Cast<APirateCharacter>(PlayersCurrentlyInTheLead[i]->GetPawn());
				if (Loser)
				{
					Loser->MulticastLostTheLead();
				}
			}
		}
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeat(1);
	}
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim(false);
	}
}

void AMainGameMode::RequestRespawn(APirateCharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}

void AMainGameMode::PlayerLeftGame(APiratePlayerState* PlayerLeaving)
{
	if (PlayerLeaving == nullptr) return;
	AMainGameState* MainGameState = GetGameState<AMainGameState>();
	if (MainGameState && MainGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		MainGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}
	APirateCharacter* CharacterLeaving = Cast<APirateCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->Elim(true);
	}
}
