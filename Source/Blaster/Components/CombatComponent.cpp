// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Blaster/character/Public/BlasterCharacter.h"
#include "Blaster/Weapons/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
}


void UCombatComponent::EquipWeapon(AWeapon* weapon)
{
	if (Character == nullptr || weapon == nullptr) return;
	
	//TODO: if exist drop
	EquippedWeapon = weapon;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	
	// attach Actor to socket
	const USkeletalMeshSocket *HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{ 
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	//set weapon ownership to our character
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->ShowPickupWidget(false);
	bIsAiming = false;
	
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::SetAiming(bool aim)
{
	//for client immediate results
	bIsAiming = aim;
	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = aim ? MaxAimWalkSpeed : MaxBaseWalkSpeed;
	}
	//
	//to update server
	ServerSetAiming(aim);

}

// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed =  MaxBaseWalkSpeed;
	}
}

//to replicate from server to Autonomous proxy and other client
void UCombatComponent::OnRep_EquippedWeapon()
{
	if (!EquippedWeapon || !Character) return;

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::ServerSetAiming_Implementation(bool isAiming)
{
	bIsAiming = isAiming;
	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = isAiming ? MaxAimWalkSpeed : MaxBaseWalkSpeed;
	}
}




// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon)
	DOREPLIFETIME(UCombatComponent, bIsAiming)
}

