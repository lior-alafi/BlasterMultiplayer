// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "Public/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}	
