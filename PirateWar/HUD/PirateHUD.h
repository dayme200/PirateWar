#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
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

private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
	
public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; };
};