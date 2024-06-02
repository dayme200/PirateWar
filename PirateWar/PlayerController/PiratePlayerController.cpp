#include "PiratePlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "PirateWar/HUD/PirateHUD.h"
#include "PirateWar/HUD/CharacterOverlay.h"
#include "PirateWar/Character/PirateCharacter.h"
#include "PirateWar/GameMode/MainGameMode.h"
#include "PirateWar/HUD/Announcement.h"


void APiratePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	PirateHUD = Cast<APirateHUD>(GetHUD());
	ServerCheckMatchState();
}

void APiratePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();
}

void APiratePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APiratePlayerController, MatchState);
}

void APiratePlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void APiratePlayerController::ServerCheckMatchState_Implementation()
{
	AMainGameMode* GameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, LevelStartingTime);

		if (PirateHUD && MatchState == MatchState::WaitingToStart)
		{
			PirateHUD->AddAnnouncement();
		}
	}
}

void APiratePlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);

	if (PirateHUD && MatchState == MatchState::WaitingToStart)
	{
		PirateHUD->AddAnnouncement();
	}
}

void APiratePlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	PirateHUD = PirateHUD == nullptr ? Cast<APirateHUD>(GetHUD()) : PirateHUD;
	bool bHUDValid = PirateHUD &&
		PirateHUD->CharacterOverlay &&
		PirateHUD->CharacterOverlay->HealthBar &&
		PirateHUD->CharacterOverlay->HealthText;
	
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		PirateHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		PirateHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		MaxHealth = MaxHealth;
	}
}

void APiratePlayerController::SetHUDScore(float Score)
{
	PirateHUD = PirateHUD == nullptr ? Cast<APirateHUD>(GetHUD()) : PirateHUD;
	bool bHUDValid = PirateHUD &&
		PirateHUD->CharacterOverlay &&
		PirateHUD->CharacterOverlay->ScoreAmount;

	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		PirateHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
	}
}

void APiratePlayerController::SetHUDDefeat(int32 Defeat)
{
	PirateHUD = PirateHUD == nullptr ? Cast<APirateHUD>(GetHUD()) : PirateHUD;
	bool bHUDValid = PirateHUD &&
		PirateHUD->CharacterOverlay &&
		PirateHUD->CharacterOverlay->DefeatAmount;

	if (bHUDValid)
	{
		FString DefeatText = FString::Printf(TEXT("%d"), Defeat);
		PirateHUD->CharacterOverlay->DefeatAmount->SetText(FText::FromString(DefeatText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeat = Defeat;
	}
}

void APiratePlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	bool bHUDValid = PirateHUD &&
		PirateHUD->CharacterOverlay &&
		PirateHUD->CharacterOverlay->AmmoAmount;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		PirateHUD->CharacterOverlay->AmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void APiratePlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	bool bHUDValid = PirateHUD &&
		PirateHUD->CharacterOverlay &&
		PirateHUD->CharacterOverlay->CarriedAmmoAmount;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		PirateHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void APiratePlayerController::SetHUDMatchCountDown(float CountDownTime)
{
	PirateHUD = PirateHUD == nullptr ? Cast<APirateHUD>(GetHUD()) : PirateHUD;
	bool bHUDValid = PirateHUD &&
		PirateHUD->CharacterOverlay &&
		PirateHUD->CharacterOverlay->MatchCountDownText;

	if (bHUDValid)
	{
		int32 Minutes = FMath::FloorToInt(CountDownTime / 60.f);
		int32 Seconds = CountDownTime - Minutes * 60;
		FString CountDownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		PirateHUD->CharacterOverlay->MatchCountDownText->SetText(FText::FromString(CountDownText));
	}
}

void APiratePlayerController::SetHUDAnnouncementCountDown(float CountDownTime)
{
	PirateHUD = PirateHUD == nullptr ? Cast<APirateHUD>(GetHUD()) : PirateHUD;
	bool bHUDValid = PirateHUD &&
		PirateHUD->Announcement &&
		PirateHUD->Announcement->WarmupTime;

	if (bHUDValid)
	{
		int32 Minutes = FMath::FloorToInt(CountDownTime / 60.f);
		int32 Seconds = CountDownTime - Minutes * 60;
		FString CountDownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		PirateHUD->Announcement->WarmupTime->SetText(FText::FromString(CountDownText));
	}
}

void APiratePlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	if (CountDownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart)
		{
			SetHUDMatchCountDown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDAnnouncementCountDown(TimeLeft);
		}
	}
	CountDownInt = SecondsLeft;
}

float APiratePlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void APiratePlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void APiratePlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (PirateHUD && PirateHUD->CharacterOverlay)
		{
			CharacterOverlay = PirateHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeat(HUDDefeat);
			}
		}
	}
}

void APiratePlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void APiratePlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void APiratePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	APirateCharacter* PirateCharacter = Cast<APirateCharacter>(InPawn);
	if (PirateCharacter)
	{
		SetHUDHealth(PirateCharacter->GetHealth(), PirateCharacter->GetMaxHealth());
	}
}

void APiratePlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;
	if (MatchState == MatchState::WaitingToStart)
	{
		
	}
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void APiratePlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void APiratePlayerController::HandleMatchHasStarted()
{
	PirateHUD = PirateHUD == nullptr ? Cast<APirateHUD>(GetHUD()) : PirateHUD;
	if (PirateHUD)
	{
		PirateHUD->AddCharacterOverlay();
		if (PirateHUD->Announcement)
		{
			PirateHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
