// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "TPSEnemyAIController.h"
#include "TPSEnemy.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	ATPSEnemy* Enemy = Cast<ATPSEnemy>(AIController->GetPawn());
	if (!Enemy)
	{
		return EBTNodeResult::Failed;
	}

	AIController->StopMovement();

	Enemy->Attack();

	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	ATPSEnemy* Enemy = Cast<ATPSEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy && Enemy->GetMesh()->GetAnimInstance())
	{	
		UAnimMontage* AttackMontage = Enemy->GetAttackMontage();
		if (AttackMontage && !Enemy->GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttackMontage))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

EBTNodeResult::Type UBTTask_Attack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		ACharacter* Enemy = Cast<ACharacter>(AIController->GetPawn());
		
		if (Enemy && Enemy->GetMesh()->GetAnimInstance())
		{
			Enemy->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f);
		}
	}

	return Super::AbortTask(OwnerComp, NodeMemory);
}


