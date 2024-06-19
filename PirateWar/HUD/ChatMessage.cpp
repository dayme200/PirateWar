#include "ChatMessage.h"
#include "Components/TextBlock.h"

void UChatMessage::SetMessageText(FString Message)
{
	FString MessageString = FString::Printf(TEXT("%s"), *Message);
	if (MessageText)
	{
		MessageText->SetText(FText::FromString(MessageString));
	}
}