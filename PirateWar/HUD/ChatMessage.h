#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatMessage.generated.h"

UCLASS()
class PIRATEWAR_API UChatMessage : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* MessageText;

	void SetMessageText(FString Message);
};
