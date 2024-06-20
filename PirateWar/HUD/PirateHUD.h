#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Types/SlateEnums.h"
#include "PirateHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairCenter;
	UTexture2D* CrosshairLeft;
	UTexture2D* CrosshairRight;
	UTexture2D* CrosshairTop;
	UTexture2D* CrosshairBottom;
	float CrosshairSpread;
	FLinearColor CrosshairColor;
};

UCLASS()
class PIRATEWAR_API APirateHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = Stat)
	TSubclassOf<class UUserWidget> CharacterOverlayClass;
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	void AddCharacterOverlay();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> ChatMessageClass;
	UPROPERTY()
	class UChatMessage* ChatMessage;
	void AddChatMessage(FString Msg = FString(""));
	TSharedPtr<class SWidget> GetChatInputTextObject();

	UPROPERTY(EditAnywhere, Category = Announcement)
	TSubclassOf<class UUserWidget> AnnouncementClass;
	UPROPERTY()
	class UAnnouncement* Announcement;
	void AddAnnouncement();

	void AddElimAnnouncement(FString Attacker, UTexture2D* WeaponT, FString Victim);
	

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	class APlayerController* OwningPlayer;
	
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UElimAnnouncement> ElimAnnouncementClass;
	
	UPROPERTY(EditAnywhere)
	float ElimAnnouncementTime = 2.5f;

	UFUNCTION()
	void ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove);

	UPROPERTY()
	TArray<UElimAnnouncement*> ElimMessages;
	
public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; };
};
