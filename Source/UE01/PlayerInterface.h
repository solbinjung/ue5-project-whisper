// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInterface.generated.h"

/**
 * 
 */
UCLASS()
class UE01_API UPlayerInterface : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UHealthBarWidget* PlayerHPBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AmmoText;

	void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);
};
