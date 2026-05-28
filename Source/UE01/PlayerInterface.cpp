// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInterface.h"
#include "Components/TextBlock.h"

void UPlayerInterface::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (AmmoText != nullptr)
	{
		FString AmmoString = FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo);
		AmmoText->SetText(FText::FromString(AmmoString));
	}
}
