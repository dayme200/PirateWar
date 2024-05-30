#pragma once

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	ETP_Left UMETA(DisplayName = "Turning Left"),
	ETP_Right UMETA(DisplayName = "Turning Right"),
	ETP_NotTurning UMETA(DisplayName = "Not Turning"),
	ETP_MAX UMETA(DisplayName = "DefaultMAX")
};