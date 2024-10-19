// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!GameState) return;
	TotalNumberOfPlayers = GameState->PlayerArray.Num();
	if (TotalNumberOfPlayers >= NumOfPlayersToStart)
	{
		auto world = GetWorld();
		if (!world) return;
		//to travel seamlessly
		bUseSeamlessTravel = true;

		world->ServerTravel(FString::Printf(TEXT("%s?listen"), *Map));
	}
}
