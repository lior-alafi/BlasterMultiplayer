// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "Public/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBlasterAnimInstance::strafe(float DeltaSeconds)
{

	FRotator AimRotation = character->GetBaseAimRotation();
	//UE_LOG(LogTemp, Warning, TEXT("Aim rotation yaw: %f"), AimRotation.Yaw)
	FRotator movemenRotation = UKismetMathLibrary::MakeRotFromX(character->GetVelocity());
	//UE_LOG(LogTemp, Warning, TEXT("movement Rotation yaw: %f"), movemenRotation.Yaw)

	//solve jittering passage of strafe animations
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(movemenRotation, AimRotation);

	//shortest path interpolation to prevent jittering
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRotation, DeltaSeconds, 6.f);
	YawOffset = DeltaRotation.Yaw;
}

void UBlasterAnimInstance::lean(float DeltaSeconds)
{
	LastFrameCharRotation = CharRotation;
	CharRotation = character->GetActorRotation();
	const auto delta = UKismetMathLibrary::NormalizedDeltaRotator(CharRotation, LastFrameCharRotation);
	//upscale value(it's super tiny)
	const float target = delta.Yaw / DeltaSeconds; 
	//interpolate to provide smoother transitions
	const float interp = FMath::FInterpTo(Lean, target, DeltaSeconds, 6.f);
	//clamp between values
	Lean = FMath::Clamp(interp, -90.f, 90.f);
}

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	character = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (character == nullptr) {
		character = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if (character == nullptr) return;

	auto velocity = character->GetVelocity();
	velocity.Z = 0.f;
	Speed = velocity.Size();

	auto movement = character->GetCharacterMovement();
	//is moving
	bIsAccelerating = movement->GetCurrentAcceleration().Size() > 0.f;
	//s.e
	bIsInAir = movement->IsFalling();

	bWeaponEquipped = character->IsWeaponEquipped();

	bIsCrouching = character->bIsCrouched;
	
	bIsAiming = character->IsAiming();

	strafe(DeltaSeconds);
	lean(DeltaSeconds);

	AO_Yaw = character->GetAimOffsetYaw();
	AO_Pitch = character->GetAimOffsetPitch();
}	
