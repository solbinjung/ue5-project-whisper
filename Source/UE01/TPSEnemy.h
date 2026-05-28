// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSEnemy.generated.h"

class UWidgetComponent;
class AAIController;
class UAnimMontage;
class UMaterialInterface;

UCLASS()
class UE01_API ATPSEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Components")
	TObjectPtr<UWidgetComponent> HealthBarWidget;

	UPROPERTY()
	TObjectPtr<AAIController> EnemyController;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemy|Stat")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Stat")
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemy|Stat")
	float MaxSpeed = 200.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|State")
	bool bIsDead = false;

	FTimerHandle HitTimerHandle;

	void ResetHitState();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Combat")
	float HeadshotMultiplier = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Combat")
	float LimbMultiplier = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Combat")
	float BaseDamage = 20.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Combat")
	float AttackRadius = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Combat")
	int32 MaxComboCount = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Combat")
	int32 AttackIndex = 0;

	UPROPERTY()
	TArray<AActor*> DamagedActors;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy|Combat")
	TObjectPtr<UMaterialInterface> BloodDecalMaterial;

public:
	virtual void Attack();

	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	void HitCheck();

	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	void CheckNextCombo();
	
	UAnimMontage* GetAttackMontage() const { return OnAttack; }

protected:
	void Die();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Enemy|Animation")
	TObjectPtr<UAnimMontage> OnHit;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy|Animation")
	TObjectPtr<UAnimMontage> OnAttack;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy|Animation")
	TObjectPtr<UAnimMontage> OnDead;

protected:
	void UpdateHealthUI();

	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy|UI")
	void ShowDamageText(float DamageAmount, FVector SpawnLocation);
};
