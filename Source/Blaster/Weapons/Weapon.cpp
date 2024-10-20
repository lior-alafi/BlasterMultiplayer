// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	WeaponState = EWeaponState::EWS_Initial;


	SK_Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SK_Weapon->SetupAttachment(RootComponent);
	SetRootComponent(SK_Weapon);
	//set collision to block so it won't fall through floor
	SK_Weapon->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//except pawn
	SK_Weapon->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	SK_Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//detect overlaps with weapon - in multiplayer it's best that 
	PickupRadius = CreateDefaultSubobject<USphereComponent>(TEXT("PickupRadius"));
	PickupRadius->SetupAttachment(RootComponent);
	//set ignored on everyone - turn on on server begin play
	PickupRadius->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickupRadius->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	//equiv to GetLocalRole() == ENetRole::ROLE_Authority
	if (HasAuthority())
	{
		PickupRadius->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PickupRadius->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

