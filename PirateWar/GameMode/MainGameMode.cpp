#include "MainGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "PirateWar/Character/PirateCharacter.h"
#include "PirateWar/PlayerState/PiratePlayerState.h"
#include "PirateWar/PlayerController/PiratePlayerController.h"

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
}

void AMainGameMode::PlayerEliminated(APirateCharacter* ElimmedCharacter, APiratePlayerController* VictimController,
                                     APiratePlayerController* AttackerController)
{
	APiratePlayerState* AttackerPlayerState = AttackerController ? Cast<APiratePlayerState>(AttackerController->PlayerState) : nullptr;
	APiratePlayerState* VictimPlayerState = VictimController ? Cast<APiratePlayerState>(VictimController->PlayerState) : nullptr;
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeat(1);
	}
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
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