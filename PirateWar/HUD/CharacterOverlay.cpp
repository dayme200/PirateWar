#include "CharacterOverlay.h"
#include "Types/SlateEnums.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "PirateWar/PlayerController/PiratePlayerController.h"

void UCharacterOverlay::NativeConstruct()
{
	Super::NativeConstruct();
}

TSharedPtr<SWidget> UCharacterOverlay::GetChatInputTextObject()
{
	return ChatInputBox->GetCachedWidget();
}

void UCharacterOverlay::CommitChatInputBox()
{
	APiratePlayerController* PirateController = Cast<APiratePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PirateController == nullptr) return;

	Text = ChatInputBox->GetText();
	
	if (Text.IsEmpty() == false)
	{
		PirateController->SendMessage(Text); // 메시지 보냄.
		ChatInputBox->SetText(FText::GetEmpty()); // 메세지 전송했으니, 비워줌.
	}
	PirateController->FocusGame(); // 다시 게임으로 포커싱.
}
