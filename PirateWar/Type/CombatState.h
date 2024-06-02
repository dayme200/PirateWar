#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unonccupired UMETA(DisplayName = "Unoccupied"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),

	ECS_MAX UMETA(DisplayName = "DefaultMax")
};