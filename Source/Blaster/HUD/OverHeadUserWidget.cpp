// Fill out your copyright notice in the Description page of Project Settings.


#include "OverHeadUserWidget.h"
#include "Components/TextBlock.h"


void UOverHeadUserWidget::SetOverheadText(FString txt)
{
	if (OverheadText)
	{
		OverheadText->SetText(FText::FromString(txt));
	}
}

void UOverHeadUserWidget::ShowNetworkRole(APawn* inPawn)
{
	
	ENetRole localRole = inPawn->GetLocalRole();
	FString role;
	switch (localRole)
	{
	case ROLE_None:
		role = FString("None");
		break;
	case ROLE_SimulatedProxy:
		role = FString("Simulated Proxy");
		break;
	case ROLE_AutonomousProxy:
		role = FString("Autonomous Proxy");
		break;
	case ROLE_Authority:
		role = FString("Authority");
		break;
	}
	
	FString  text = FString::Printf(TEXT("Local Role: %s"), *role);
	SetOverheadText(text);
}


void UOverHeadUserWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
