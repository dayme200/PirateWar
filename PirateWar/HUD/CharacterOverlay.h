#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

UCLASS()
class PIRATEWAR_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ShieldBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ShieldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RedTeamScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BlueTeamScore;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefeatAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchCountDownText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GrenadeAmountText;

	UPROPERTY(meta = (BindWidget))
	class UImage* HighPingImage;

	UPROPERTY(meta = (BindWidget))
	UImage* MainWeaponImg;
	UPROPERTY(meta = (BindWidget))
	UImage* SubWeaponImg;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* HighPingAnimation;

	UPROPERTY(meta = (BindWidget))
	class UBorder* ChatBorder;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UScrollBox* Chat_ScrollBox;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UEditableTextBox* ChatInputBox;

	TSharedPtr<class SWidget> GetChatInputTextObject();

	UFUNCTION(BlueprintCallable)
	void CommitChatInputBox();

	UPROPERTY()
	FText Text;
};