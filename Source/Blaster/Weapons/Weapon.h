// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8 {
	EWS_Initial UMETA(DisplayName="Initial State"),
	EWS_Equipped UMETA(DisplayName="Equipped"),
	EWS_Dropped UMETA(DisplayName="Dropped"),

	EWS_Max UMETA(DisplayName = "DefaultMax") //max # of states
};

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere,Category="Weapon Properties")
	USkeletalMeshComponent *SK_Weapon;


	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* PickupRadius;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* pickupWidget;
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void PickupBeginOverlap(
		UPrimitiveComponent *OverlappedComponent,
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult
	);

	UFUNCTION()
	virtual void PickupEndOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex
	);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ShowPickupWidget(bool bShowWidget);
};
