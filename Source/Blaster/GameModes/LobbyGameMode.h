// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly,Category="Players",meta=(AllowPrivateAccess="true"))
	int32 TotalNumberOfPlayers;
	
protected:
	int32 NumOfPlayersToStart = 3;

	FString Map{ TEXT("/Game/maps/BlasterMap") };
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
