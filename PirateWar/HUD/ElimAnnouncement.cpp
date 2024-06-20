#include "ElimAnnouncement.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UElimAnnouncement::SetElimAnnouncementText(FString AttackerName, class UTexture2D* WeaponT, FString VictimName)
{
	if (AttackerText && WeaponTexture && ElimerText)
	{
		AttackerText->SetText(FText::FromString(AttackerName));
		WeaponTexture->SetBrushFromTexture(WeaponT);
		ElimerText->SetText(FText::FromString(VictimName));
	}
}
