#include "PiratePlayerController.h"

#include "Components/Image.h"
#include "Net/UnrealNetwork.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "PirateWar/HUD/PirateHUD.h"
#include "PirateWar/HUD/Announcement.h"
#include "PirateWar/HUD/CharacterOverlay.h"
#include "PirateWar/GameMode/MainGameMode.h"
#include "PirateWar/Character/PirateCharacter.h"
#include "PirateWar/Component/CombatComponent.h"
#include "PirateWar/GameState/MainGameState.h"
#include "PirateWar/PlayerState/PiratePlayerState.h"


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
	CheckPing(DeltaTime);
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

void APiratePlayerController::HighPingWarning()
{
	PirateHUD = PirateHUD == nullptr ? Cast<APirateHUD>(GetHUD()) : PirateHUD;
	bool bHUDValid = PirateHUD &&
		PirateHUD->CharacterOverlay &&
		PirateHUD->CharacterOverlay->HighPingImage &&
		PirateHUD->CharacterOverlay->HighPingAnimation;
	
	if (bHUDValid)
	{
		PirateHUD->CharacterOverlay->HighPingImage->SetOpacity(1.f);
		PirateHUD->CharacterOverlay->PlayAnimation(PirateHUD->CharacterOverlay->HighPingAnimation, 0.f, 5);
	}
}

void APiratePlayerController::StopHighPingWarning()
{
	PirateHUD = PirateHUD == nullptr ? Cast<APirateHUD>(GetHUD()) : PirateHUD;
	bool bHUDValid = PirateHUD &&
		PirateHUD->CharacterOverlay &&
		PirateHUD->CharacterOverlay->HighPingImage &&
		PirateHUD->CharacterOverlay->HighPingAnimation;
	
	if (bHUDValid)
	{
		PirateHUD->CharacterOverlay->HighPingImage->SetOpacity(1.f);
		if (PirateHUD->CharacterOverlay->IsAnimationPlaying(PirateHUD->CharacterOverlay->HighPingAnimation))
		{
			PirateHUD->CharacterOverlay->StopAnimation(PirateHUD->CharacterOverlay->HighPingAnimation);
		}
	}
}

void APiratePlayerController::CheckPing(float DeltaTime)
{
	HighPingRunningTime += DeltaTime;
	if (HighPingRunningTime > CheckPingFrequency)
	{
		PlayerState = PlayerState == nullptr ? TObjectPtr<APlayerState>(GetPlayerState<APlayerState>()) : PlayerState;
		if (PlayerState)
		{
			if (PlayerState->GetCompressedPing() * 4 > HighPingThreshold)
			{
				HighPingWarning();
				PingAnimationRunningTime = 0.f;
			}
		}
		HighPingRunningTime = 0.f;
	}
	
	bool bHighPingAnimationPlaying = PirateHUD &&
		PirateHUD->CharacterOverlay &&
		PirateHUD->CharacterOverlay->HighPingAnimation &&
		PirateHUD->CharacterOverlay->IsAnimationPlaying(PirateHUD->CharacterOverlay->HighPingAnimation);
	
	if (bHighPingAnimationPlaying)
	{
		PingAnimationRunningTime += DeltaTime;
		if (PingAnimationRunningTime > HighPingDuration)
		{
			StopHighPingWarning();
		}
	}
}

void APiratePlayerController::ServerCheckMatchState_Implementation()
{
	AMainGameMode* GameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);

		if (PirateHUD && MatchState == MatchState::WaitingToStart)
		{
			PirateHUD->AddAnnouncement();
		}
	}
}

void APiratePlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
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
		bInitializeHealth = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void APiratePlayerController::SetHUDShield(float Shield, float MaxShield)
{
	PirateHUD = PirateHUD == nullptr ? Cast<APirateHUD>(GetHUD()) : PirateHUD;
	bool bHUDValid = PirateHUD &&
		PirateHUD->CharacterOverlay &&
		PirateHUD->CharacterOverlay->ShieldBar &&
		PirateHUD->CharacterOverlay->ShieldText;
	
	if (bHUDValid)
	{
		const float ShieldPercent = Shield / MaxShield;
		PirateHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
		FString ShieldText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Shield), FMath::CeilToInt(MaxShield));
		PirateHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));
	}
	else
	{
		bInitializeShield = true;
		HUDShield = Shield;
		HUDMaxShield = MaxShield;
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
		bInitializeScore = true;
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
		bInitializeDefeat = true;
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
	else
	{
		bInitializeWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;
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
	else
	{
		bInitializeCarriedAmmo = true;
		HUDCarriedAmmo = Ammo;
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
		if (CooldownTime < 0.f)
		{
			PirateHUD->CharacterOverlay->MatchCountDownText->SetText(FText());
			return;
		}
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
		if (CountDownTime < 0.f)
		{
			PirateHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountDownTime / 60.f);
		int32 Seconds = CountDownTime - Minutes * 60;
		FString CountDownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		PirateHUD->Announcement->WarmupTime->SetText(FText::FromString(CountDownText));
	}
}

void APiratePlayerController::SetHUDGrenade(int32 Grenade)
{
	bool bHUDValid = PirateHUD &&
		PirateHUD->CharacterOverlay &&
		PirateHUD->CharacterOverlay->GrenadeAmountText;

	if (bHUDValid)
	{
		FString GrenadeText = FString::Printf(TEXT("%d"), Grenade);
		PirateHUD->CharacterOverlay->GrenadeAmountText->SetText(FText::FromString(GrenadeText));
	}
	else
	{
		bInitializeGrenade = true;
		HUDGrenade = Grenade;
	}
}

void APiratePlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	if (HasAuthority())
	{
		MainGameMode = MainGameMode == nullptr ? Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this)) : MainGameMode;
		if (MainGameMode)
		{
			SecondsLeft = FMath::CeilToInt(MainGameMode->GetCountDownTime() + LevelStartingTime);
		}
	}
	
	if (CountDownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
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
				if (bInitializeHealth) SetHUDHealth(HUDHealth, HUDMaxHealth);
				if (bInitializeShield) SetHUDShield(HUDShield, HUDMaxShield);
				if (bInitializeScore) SetHUDScore(HUDScore);
				if (bInitializeDefeat) SetHUDDefeat(HUDDefeat);
				if (bInitializeCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
				if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);
				APirateCharacter* PirateCharacter = Cast<APirateCharacter>(GetPawn());
				if (PirateCharacter && PirateCharacter->GetCombat()->GetGrenade())
				{
					if (bInitializeGrenade) SetHUDGrenade(PirateCharacter->GetCombat()->GetGrenade());
				}
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
	SingleTripTime = 0.5f + RoundTripTime;
	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;
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
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void APiratePlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void APiratePlayerController::HandleMatchHasStarted()
{
	PirateHUD = PirateHUD == nullptr ? Cast<APirateHUD>(GetHUD()) : PirateHUD;
	if (PirateHUD)
	{
		if (PirateHUD->CharacterOverlay == nullptr) PirateHUD->AddCharacterOverlay();
		if (PirateHUD->Announcement)
		{
			PirateHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void APiratePlayerController::HandleCooldown()
{
	PirateHUD = PirateHUD == nullptr ? Cast<APirateHUD>(GetHUD()) : PirateHUD;
	if (PirateHUD)
	{
		PirateHUD->CharacterOverlay->RemoveFromParent();
		bool bHUDValid = PirateHUD->Announcement &&
			PirateHUD->Announcement->AnnouncementText &&
			PirateHUD->Announcement->InfoText;
		
		if (bHUDValid)
		{
			PirateHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match Starts In:");
			PirateHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));
			
			AMainGameState* MainGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(this));
			APiratePlayerState* PiratePlayerState = GetPlayerState<APiratePlayerState>();
			if (MainGameState && PiratePlayerState)
			{
				TArray<APiratePlayerState*> TopPlayers = MainGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString("There is no winner");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == PiratePlayerState)
				{
					InfoTextString = FString("You are winner!");
				}
				else if (TopPlayers.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoTextString = FString("Players tied for the win:\n");
					for (auto Tiedplayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *Tiedplayer->GetPlayerName()));
					}
				}
				PirateHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
		}
	}
	APirateCharacter* PirateCharacter = Cast<APirateCharacter>(GetPawn());
	if (PirateCharacter && PirateCharacter->GetCombat())
	{
		PirateCharacter->bDisableGameplay = true;
		PirateCharacter->GetCombat()->FireButtonPressed(false);
	}
}
