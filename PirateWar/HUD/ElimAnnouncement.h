#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ElimAnnouncement.generated.h"

UCLASS()
class PIRATEWAR_API UElimAnnouncement : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetElimAnnouncementText(FString AttackerName, class UTexture2D* WeaponT, FString VictimName);

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* AnnouncementBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AttackerText;

	UPROPERTY(meta = (BindWidget))
	class UImage* WeaponTexture;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ElimerText;
};
