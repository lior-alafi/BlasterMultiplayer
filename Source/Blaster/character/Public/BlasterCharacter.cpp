	// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include  "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h" // for overhead
#include "GameFramework/PlayerState.h"
#include "Blaster/HUD/OverHeadUserWidget.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapons/Weapon.h"
#include "Blaster/Components/CombatComponent.h"
#include "Kismet/KismetMathLibrary.h"

void ABlasterCharacter::OnRep_OverlappedWeapon(AWeapon* previousOverlappedWeapon)
{
	if (OverlappedWeapon)
	{
		OverlappedWeapon->ShowPickupWidget(true);
	}
	if (previousOverlappedWeapon) {
		previousOverlappedWeapon->ShowPickupWidget(false);
	}
}

void ABlasterCharacter::CalculateAimOffset(float deltaTime)
{
	float Speed = GetVelocity().Size();
	FRotator currentAimRotator = GetBaseAimRotation();

	if (Speed > 0.f || GetCharacterMovement()->IsFalling()) 
	{
		// +++ changed to true ADDED +++
		bUseControllerRotationYaw = true;

		LastAimRotator = currentAimRotator;
		OffsetYaw = 0.f;

		TurningInPlace = ETurningInPlace::ETIP_NoRotation;
	}
	else if(Speed == 0.f && !GetCharacterMovement()->IsFalling())
	{
		bUseControllerRotationYaw = true;

		FRotator lastYaw = FRotator{ 0.f,LastAimRotator.Yaw,0.f };
		FRotator currYaw = FRotator{ 0.f,currentAimRotator.Yaw,0.f };
		OffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(currYaw, lastYaw).Yaw;
		
		// +++ initialize it ADDED +++
		if (TurningInPlace == ETurningInPlace::ETIP_NoRotation) {
			InterpAO_Yaw = OffsetYaw;
		}
	

		TurnInPlace(deltaTime);
	}
	OffsetPitch = currentAimRotator.Pitch;
	/*since rotators are compressed to 16byte pitch value range is[0, 360]
	 (it's fine locally) so let's map [270,360) range to [-90,0)
	*/

	if (!IsLocallyControlled() && OffsetPitch > 90.f) {
		FVector2D InputRange{ 270.f,360.f }, OutputRange{ -90.f,0.f };
		OffsetPitch = FMath::GetMappedRangeValueClamped(InputRange, OutputRange, OffsetPitch);
	}

	UE_LOG(LogTemp, Warning, TEXT("yaw: %f, pitch: %f"), OffsetYaw, OffsetPitch);
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (90.f < OffsetYaw)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (OffsetYaw < -90.f)
	{
		TurningInPlace =  ETurningInPlace::ETIP_Left;
	}

	//if we are turning interpolate interpAO +++ADDED
	if (TurningInPlace != ETurningInPlace::ETIP_NoRotation) {
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, RotationInterpolationRate);
		OffsetYaw = InterpAO_Yaw;
		//check if we should reset turn
		if (FMath::Abs(OffsetYaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NoRotation;
			//reset aim rotation
			LastAimRotator = GetBaseAimRotation();
		}
	}
}


// Sets default values
ABlasterCharacter::ABlasterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	//rotate the springarm with the character when it's rotating
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//connect camera to spring arm via socket
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//already attached to spring arm that follows rotation
	FollowCamera->bUsePawnControlRotation = false;

	// these two will stop camera to rotate with the camera (now allows to rotate around the character)
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//widget 
	overheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	overheadWidget->SetupAttachment(RootComponent);

	//combat component
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	//since combat will have replicated members it must be set as replicated to
	Combat->SetIsReplicated(true);
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	TurningInPlace = ETurningInPlace::ETIP_NoRotation;

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 35.f;
}

// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ABlasterCharacter::MoveForward(float val)
{
	if (Controller != nullptr && val != 0.f) {
		/* dont use auto fwdVector = GetActorForwardVector();
			instead use this - forward vector like this is considering controller rotations
		*/
		const FRotator currentRotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector direction(FRotationMatrix(currentRotation).GetUnitAxis(EAxis::X));

		AddMovementInput(direction, val);
	}
}

void ABlasterCharacter::MoveSide(float val)
{
	if (Controller != nullptr && val != 0.f) {
		
		const FRotator currentRotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
		//sort of like GetActorRightVector() but relative to controller
		const FVector direction(FRotationMatrix(currentRotation).GetUnitAxis(EAxis::Y));

		AddMovementInput(direction, val);
	}
}

void ABlasterCharacter::LookUp(float val)
{
	AddControllerPitchInput(val);
}

void ABlasterCharacter::Turn(float val)
{
	AddControllerYawInput(val);
}

void ABlasterCharacter::EquppiedButtonPressed()
{

	if (Combat) {
		/*
		if happens on server player - cool let's equip otherwise call the rpc
		- it will surely happen on server but will also allow client to equip

		*/
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappedWeapon);
		}
		else {
			ServerEquppiedButtonPressed();
		}
		
	}
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if (CanCrouch()) {
		Crouch();	
	}
	else {
		UnCrouch();
	}
}

void ABlasterCharacter::AimButtonPressed()
{
	if (Combat == nullptr) return;
	Combat->SetAiming(true);
}

void ABlasterCharacter::AimButtonReleased()
{
	if (Combat == nullptr) return;
	Combat->SetAiming(false);
}

void ABlasterCharacter::ServerEquppiedButtonPressed_Implementation()
{
	Combat->EquipWeapon(OverlappedWeapon);
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//register replication variable
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappedWeapon, COND_OwnerOnly);
}

void ABlasterCharacter::SetOverlappedWeapon(AWeapon* overlappedWeapon)
{
	if (OverlappedWeapon) {
		OverlappedWeapon->ShowPickupWidget(false);
	}
	OverlappedWeapon = overlappedWeapon;

	/*
	* in our case we bound to this method and call it only from server 
	* so if IsLocallyControlled() is true then we just show the pickupwidget
	* 
	*/
	if (IsLocallyControlled())
	{
		if (overlappedWeapon) {
			overlappedWeapon->ShowPickupWidget(true);
		}	
	}
}

bool ABlasterCharacter::IsWeaponEquipped() const {
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming() const
{
	return (Combat && Combat->bIsAiming);
}

float ABlasterCharacter::GetAimOffsetYaw() const
{
	return OffsetYaw;
}

float ABlasterCharacter::GetAimOffsetPitch() const
{
	return OffsetPitch;
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateAimOffset(DeltaTime);
}

// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis("MoveSide", this, &ThisClass::MoveSide);
	PlayerInputComponent->BindAxis("Turn", this, &ThisClass::Turn);
	PlayerInputComponent->BindAxis("LookUpDown", this, &ThisClass::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ThisClass::Jump);
	PlayerInputComponent->BindAction("EquipWeap", IE_Pressed, this, &ThisClass::EquppiedButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ThisClass::CrouchButtonPressed);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ThisClass::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ThisClass::AimButtonReleased);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
}
