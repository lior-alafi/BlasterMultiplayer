// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/Models/TurningInPlace.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent *CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = Combat)
	class UCombatComponent* Combat;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	class UWidgetComponent* overheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappedWeapon)
	class AWeapon* OverlappedWeapon;

	
	//Aim Offset 
	float OffsetYaw;
	float OffsetPitch;

	//rep notify 
	UFUNCTION()
	void OnRep_OverlappedWeapon(AWeapon* previousOverlappedWeapon);

	void CalculateAimOffset(float deltaTime);
	FRotator LastAimRotator;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);
	//used for interpolation of offsetYaw when we rotate beyond +-90
	float InterpAO_Yaw;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float RotationInterpolationRate = 5.f;

public:
	// Sets default values for this character's properties
	ABlasterCharacter();

	//virtual void R
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void MoveForward(float val);
	virtual void MoveSide(float val);
	virtual void LookUp(float val);
	virtual void Turn(float val);


	void EquppiedButtonPressed();
	void CrouchButtonPressed();

	void AimButtonPressed();
	void AimButtonReleased();

	UFUNCTION(Server,Reliable)
	void ServerEquppiedButtonPressed();
public:	
	//to register variables replicated 
	// to do that:
	//OREPLIFETIME(<currentclass>,<replicated class>)
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetOverlappedWeapon(AWeapon* overlappedWeapon);
	
	bool IsWeaponEquipped() const;
	bool IsAiming() const;

	float GetAimOffsetYaw() const;
	float GetAimOffsetPitch() const;

	AWeapon* GetEquippedWeapon();

	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
};
