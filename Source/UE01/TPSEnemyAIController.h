// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TPSAIControllerInterface.h"
#include "TPSEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class UE01_API ATPSEnemyAIController : public AAIController, public ITPSAIControllerInterface
{
	GENERATED_BODY()
public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual AActor* GetTarget_Implementation() override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<AActor> CurrentTarget;
};
