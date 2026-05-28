// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ATPSWeapon;
class ITPSWeaponInterface;
class UPlayerInterface;

UCLASS()
class UE01_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;

public:
	void OnActionAStarted();
	void OnActionACompleted();
	void OnActionBStarted();
	void OnActionBCompleted();

public:
	void EquipWeapon(int32 Index);
	void ReloadWeapon();

	void PlayHitReactMontage(AActor* Attacker);

protected:
	void BeginAiming();
	void EndAiming();
	void TryEndAimingStance();

	UFUNCTION()
	TArray<FName> GetBoneNamesOfMesh() { return GetMesh()->GetAllSocketNames(); }

public:
	UPROPERTY(EditDefaultsOnly, EditFixedSize, Category = "Combat|Weapon")
	TArray<TSubclassOf<ATPSWeapon>> WeaponClasses;

	UPROPERTY(VisibleAnywhere, Category = "Combat|Weapon")
	TArray<TObjectPtr<ATPSWeapon>> Weapons;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Weapon")
	TObjectPtr<ATPSWeapon> Weapon;

	UPROPERTY(EditDefaultsOnly, meta = (GetOptions = GetBoneNamesOfMesh), Category = "Combat|Weapon")
	FName SocketOnWeaponAttached;

	FTimerHandle FireStanceTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Animation")
	TObjectPtr<UAnimMontage> OnHit;


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsAiming = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsZooming = false;

protected:
	bool bIsActionBHeld = false;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Camera|Zoom")
	float DefaultFOV = 90.f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera|Zoom")
	float AimFOV = 60.f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera|Zoom")
	float ZoomInterpSpeed = 20.f;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Player|Stat")
	float MaxHealth = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Player|Stat")
	float MaxSpeed = 400.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	float CurrentHealth;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|State")
	bool bIsDead = false;
	
	void Die();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Player|UI")
	TSubclassOf<UPlayerInterface> PlayerHUDClass;

	UPROPERTY()
	class UPlayerInterface* PlayerHUD;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Player|UI")
	void ShowDamageUI();

};
