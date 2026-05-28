// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSEnemy.h"
#include "AIController.h"
#include "CommonMacros.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "HealthBarWidget.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/DamageEvents.h"
#include "BrainComponent.h"

#define DRAW_DEBUG

// Sets default values
ATPSEnemy::ATPSEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AssignDefaultSubobject(HealthBarWidget)->SetupAttachment(RootComponent);

	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetDrawSize(FVector2D(125.0f, 10.0f));
	HealthBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 125.0f));

	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	
	GetCharacterMovement()->BrakingDecelerationWalking = 500.0f;
	GetCharacterMovement()->GroundFriction = 5.0f;
}

// Called when the game starts or when spawned
void ATPSEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	EnemyController = Cast<AAIController>(GetController());

	CurrentHealth = MaxHealth;
	UpdateHealthUI();
}

// Called every frame
void ATPSEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATPSEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ATPSEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (CurrentHealth <= 0.0f)
	{
		return 0.0f;
	}

	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	FVector TextSpawnLocation = GetActorLocation();

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);

		TextSpawnLocation = PointDamageEvent->HitInfo.ImpactPoint;

		FString HitBoneNameStr = PointDamageEvent->HitInfo.BoneName.ToString();

		if (HitBoneNameStr.Contains(TEXT("head"), ESearchCase::IgnoreCase))
		{
			HeadshotMultiplier = 2.0f;
		}
		else if (HitBoneNameStr.EndsWith(TEXT("_l"), ESearchCase::IgnoreCase) ||
			HitBoneNameStr.EndsWith(TEXT("_r"), ESearchCase::IgnoreCase))
		{
			LimbMultiplier = 0.5f;
		}
	}

	CurrentHealth -= FinalDamage;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
	else
	{
		if (OnHit)
		{
			PlayAnimMontage(OnHit);

			if (EnemyController)
			{
				EnemyController->StopMovement();

				if (EnemyController->GetBlackboardComponent())
				{
					EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("IsHit"), true);
				}
			}

			GetWorldTimerManager().SetTimer(HitTimerHandle, this, &ATPSEnemy::ResetHitState, 0.8f, false);
		}
	}

	UpdateHealthUI();

	if (FinalDamage > 0.0f)
	{
		ShowDamageText(FinalDamage, TextSpawnLocation);
	}

	if (BloodDecalMaterial)
	{
		FVector StartPoint = GetActorLocation();
		FVector EndPoint = StartPoint - FVector(0.0f, 0.0f, 500.0f);

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_Visibility, QueryParams))
		{
			FRotator DecalRotation = (-HitResult.ImpactNormal).Rotation();

			DecalRotation.Roll = FMath::RandRange(-180.0f, 180.0f);

			UGameplayStatics::SpawnDecalAtLocation(
				GetWorld(),
				BloodDecalMaterial,
				FVector(80.0f, 80.0f, 80.0f),
				HitResult.ImpactPoint,
				DecalRotation,
				10.0f
			);
		}
	}

	return FinalDamage;
}

void ATPSEnemy::ResetHitState()
{
	if (EnemyController && EnemyController->GetBlackboardComponent())
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("IsHit"), false);
	}
}

void ATPSEnemy::Attack()
{
	AttackIndex = 0;
	DamagedActors.Empty();

	if (OnAttack)
	{	
		FName SectionName = FName(*FString::Printf(TEXT("AttackCombo%d"), AttackIndex));
		PlayAnimMontage(OnAttack, 1.0f, SectionName);
	}
}

void ATPSEnemy::HitCheck()
{
	if (!GetWorld() || !GetMesh())
	{
		return;
	}

	FVector StartPoint = GetMesh()->GetSocketLocation(FName("RightHandSocket"));

	FVector EndPoint = StartPoint + GetActorForwardVector() * 100.0f;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult HitResult;

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		StartPoint,
		EndPoint,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(AttackRadius),
		QueryParams
	);

#if defined DRAW_DEBUG
	FColor DrawColor = bHit ? FColor::Red : FColor::Green;
	DrawDebugSphere(GetWorld(), EndPoint, AttackRadius, 16, DrawColor, false, 2.0f);
#endif

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor != nullptr && !DamagedActors.Contains(HitActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Apply Damage to: %s"), *HitActor->GetName());

			UGameplayStatics::ApplyDamage(
				HitActor,
				BaseDamage,
				GetController(),
				this,
				UDamageType::StaticClass()
			);

			DamagedActors.Add(HitActor);
		}
	}
}

void ATPSEnemy::CheckNextCombo()
{
	if (!EnemyController || !EnemyController->GetBlackboardComponent()) return;

	bool bShouldContinueCombo = EnemyController->GetBlackboardComponent()->GetValueAsBool(TEXT("IsInRange"));

	if (bShouldContinueCombo)
	{
		DamagedActors.Empty();

		AttackIndex = (AttackIndex + 1) % MaxComboCount;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && AnimInstance->Montage_IsPlaying(OnAttack))
		{
			FName CurrentSectionName = AnimInstance->Montage_GetCurrentSection(OnAttack);

			FName NextSectionName = FName(*FString::Printf(TEXT("AttackCombo%d"), AttackIndex));

			AnimInstance->Montage_SetNextSection(CurrentSectionName, NextSectionName, OnAttack);
		}
	}
	else
	{
		AttackIndex = 0;
		DamagedActors.Empty();
	}
}

void ATPSEnemy::Die()
{
	bIsDead = true;

	if (OnDead)
	{
		PlayAnimMontage(OnDead);
	}

	if (EnemyController && EnemyController->GetBlackboardComponent())
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("IsDead"), bIsDead);
	}

	if (EnemyController && EnemyController->GetBrainComponent())
	{
		EnemyController->GetBrainComponent()->StopLogic("Enemy is Dead");
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
	
	SetLifeSpan(3.0f);
}

void ATPSEnemy::UpdateHealthUI()
{
	if (HealthBarWidget)
	{
		UHealthBarWidget* HPWidget = Cast<UHealthBarWidget>(HealthBarWidget->GetUserWidgetObject());

		if (HPWidget)
		{
			HPWidget->UpdateHP(CurrentHealth, MaxHealth);
			HPWidget->SetBarColor(FColor::Red);
		}
	}
}
