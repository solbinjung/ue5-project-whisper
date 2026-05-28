// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarWidget.h"
#include "Components/ProgressBar.h"

void UHealthBarWidget::UpdateHP(float CurrentHP, float MaxHP)
{
	if (HPBar != nullptr && MaxHP > 0)
	{
		HPBar->SetPercent(CurrentHP / MaxHP);
	}
}

void UHealthBarWidget::SetBarColor(FLinearColor NewColor)
{
	if (HPBar != nullptr)
	{
		HPBar->SetFillColorAndOpacity(NewColor);
	}
}