// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BlasterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UBlasterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
private:

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed;
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsCrouching;

	UPROPERTY(BlueprintReadOnly,Category="Weapon Status",meta=(AllowPrivateAccess = "true"))
	bool bWeaponEquipped;


	UPROPERTY(BlueprintReadOnly, Category = "Weapon Status", meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Status", meta = (AllowPrivateAccess = "true"))
	float AO_Yaw;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Status", meta = (AllowPrivateAccess = "true"))
	float AO_Pitch;
	// strafe
	UPROPERTY(BlueprintReadOnly,Category= "Movement", meta = (AllowPrivateAccess="true"))
	float YawOffset; 

	void strafe(float DeltaSeconds);
	FRotator DeltaRotation;


	//lean
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Lean;
	  
	void lean(float DeltaSeconds);

	FRotator LastFrameCharRotation;
	FRotator CharRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Status", meta = (AllowPrivateAccess = "true"))
	FTransform 	LeftHandTransform;
	void handleWeaponHolding(float DeltaSeconds);

	class AWeapon* currentWeapon;
	//s.e
	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess="true"))
	class ABlasterCharacter* character;

	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
