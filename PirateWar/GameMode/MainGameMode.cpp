#include "MainGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "PirateWar/Character/PirateCharacter.h"
#include "PirateWar/PlayerState/PiratePlayerState.h"
#include "PirateWar/PlayerController/PiratePlayerController.h"

void AMainGameMode::PlayerEliminated(APirateCharacter* ElimmedCharacter, APiratePlayerController* VictimController,
                                     APiratePlayerController* AttackerController)
{
	APiratePlayerState* AttackerPlayerState = AttackerController ? Cast<APiratePlayerState>(AttackerController->PlayerState) : nullptr;
	APiratePlayerState* VictimPlayerState = VictimController ? Cast<APiratePlayerState>(VictimController->PlayerState) : nullptr;
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
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

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();

	bUseSeamlessTravel = true;
}
