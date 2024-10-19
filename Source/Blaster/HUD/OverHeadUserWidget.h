// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverHeadUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UOverHeadUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	//tie between OverheadText in bp to this variable
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* OverheadText;

	void SetOverheadText(FString txt);

	UFUNCTION(BlueprintCallable)
	void ShowNetworkRole(APawn* inPawn);

protected:
	virtual void NativeDestruct() override;
};
