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

// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
}