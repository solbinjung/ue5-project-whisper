// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSWeaponInterface.h"
#include "TPSWeapon.generated.h"

class UNiagaraSystem;

UCLASS(Abstract)
class UE01_API ATPSWeapon : public AActor, public ITPSWeaponInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPSWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual void BeginActionA() override { BeginFire(); }
	virtual void EndActionA() override { EndFire(); }
	virtual void BeginSubAction() override { BeginReload(); }
	virtual void EndSubAction() override { EndReload(); }

	UFUNCTION(BlueprintCallable)
	void EndReload();
	void UpdateAmmoUI();
	void CancelReload();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Setup")
	TSubclassOf<UAnimInstance> UserAnimClass;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

protected:
	virtual void BeginFire();
	virtual void EndFire();
	virtual void Fire();
	virtual void BeginReload();
	
	UFUNCTION()
	TArray<FName> GetBoneNamesOfMesh() { return Mesh->GetAllSocketNames();  }

protected:
	UPROPERTY()
	class ATPSCharacter* CashedOwner;	

	UPROPERTY(EditDefaultsOnly, meta = (Getoptions = GetBoneNamesOfMesh), Category = "Weapon|Setup")
	FName MuzzleSocket;

	UPROPERTY(EditDefaultsOnly, meta = (Getoptions = GetBoneNamesOfMesh), Category = "Weapon|Setup")
	FName ShellEjectSocket;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|FX")
	TObjectPtr<UNiagaraSystem> MuzzleFlashSystem;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|FX")
	TObjectPtr<UNiagaraSystem> TracerSystem;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|FX")
	TObjectPtr<UStaticMesh> ShellEjectMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|FX")
	TObjectPtr<UNiagaraSystem> ShellEjectSystem;

	UPROPERTY(EditAnywhere, Category = "Weapon|FX")
	TSubclassOf<class UCameraShakeBase> FireCameraShakeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|FX")
	TObjectPtr<class UMaterialInterface> BulletDecalMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stat")
	bool bIsAutomatic;

	FTimerHandle FireTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stat")
	float FireRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stat")
	float BaseDamage;

	float DamageMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stat")
	float MaxRange = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stat")
	int32 PelletCount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stat")
	float BaseSpread;

	float CurrentSpread;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stat")
	float MaxSpread;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stat")
	float SpreadIncreasePerShot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stat")
	float RecoilForce;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Ammo")
	bool bIsReloading = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MaxAmmo = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 CurrentAmmo;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Animation")
	TObjectPtr<UAnimMontage> OnEquipped;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Animation")
	TObjectPtr<UAnimMontage> OnFire;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Animation")
	TObjectPtr<UAnimMontage> OnReload;
};
