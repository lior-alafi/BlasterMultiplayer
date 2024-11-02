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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//friend cause we'll need to access private members and such
	friend class ABlasterCharacter;

	void EquipWeapon(AWeapon* weapon);

	void SetAiming(bool aim);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool isAiming);
private:

	//replicate strafing to all clients
	UFUNCTION()
	void OnRep_EquippedWeapon();


	//so we can equip and unequip weapon
	UPROPERTY(ReplicatedUsing= OnRep_EquippedWeapon)
    AWeapon* EquippedWeapon; 

	UPROPERTY(Replicated)
	bool bIsAiming;
	//hold the relevant character pointer
	ABlasterCharacter* Character; 




};
