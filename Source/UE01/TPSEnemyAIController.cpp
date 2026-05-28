// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSEnemyAIController.h"

void ATPSEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset != nullptr)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

AActor* ATPSEnemyAIController::GetTarget_Implementation()
{
	return CurrentTarget;
}
