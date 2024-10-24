// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blaster/character/Public/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"

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


	pickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupMessage"));
	pickupWidget->SetupAttachment(SK_Weapon);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, WeaponState);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	//equiv to GetLocalRole() == ENetRole::ROLE_Authority
	if (HasAuthority())
	{
		PickupRadius->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PickupRadius->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		//register delegates
		PickupRadius->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::PickupBeginOverlap);
		PickupRadius->OnComponentEndOverlap.AddDynamic(this, &AWeapon::PickupEndOverlap);
	}
	if (pickupWidget) {
		pickupWidget->SetVisibility(false);
	}
}

void AWeapon::PickupBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto character = Cast<ABlasterCharacter>(OtherActor);
	if (character == nullptr) return;

	character->SetOverlappedWeapon(this);
	if (character->IsLocallyControlled())
	{
		ShowPickupWidget(true);
	}
}

void AWeapon::PickupEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto character = Cast<ABlasterCharacter>(OtherActor);
	if (character == nullptr) return;

	
	character->SetOverlappedWeapon(nullptr);
	if (character->IsLocallyControlled())
	{
		ShowPickupWidget(false);
	}
	
}

void AWeapon::OnRep_WeaponState(EWeaponState previousWeaponState)
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		PickupRadius->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Initial:

		break;
	case EWeaponState::EWS_Dropped:

		break;
	}
}



void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (pickupWidget == nullptr) return;
	pickupWidget->SetVisibility(bShowWidget);
}

void AWeapon::SetWeaponState(EWeaponState state)
{
	WeaponState = state;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		//stop collision on the server since BlasterCharacter:EquipWeapon happens only on server
		PickupRadius->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Initial:

		break;
	case EWeaponState::EWS_Dropped:

		break;
	}
}

