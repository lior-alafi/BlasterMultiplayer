// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


class AWeapon;
class ABlasterCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//friend cause we'll need to access private members and such
	friend class ABlasterCharacter;

	void EquipWeapon(AWeapon* weapon);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	

private:
    AWeapon* EquippedWeapon; //so we can equip and unequip weapon
	ABlasterCharacter* Character; //hold the relevant character pointer
};
