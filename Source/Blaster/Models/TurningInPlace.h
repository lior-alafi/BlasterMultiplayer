#pragma once

UENUM(BlueprintType)
enum class ETurningInPlace : uint8 
{
	ETIP_NoRotation UMETA(DisplayName = "No Rotation"),
	ETIP_Left UMETA(DisplayName = "Left Rotation"),
	ETIP_Right UMETA(DisplayName = "Right Rotation"),
	ETIP_MaxItems UMETA(DisplayName = "DefaultMax")
};