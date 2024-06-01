#include "PiratePlayerController.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "PirateWar/HUD/PirateHUD.h"
#include "PirateWar/HUD/CharacterOverlay.h"


void APiratePlayerController::BeginPlay()
{
	Super::BeginPlay();

	PirateHUD = Cast<APirateHUD>(GetHUD());
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
}
