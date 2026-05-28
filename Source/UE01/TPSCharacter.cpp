// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCharacter.h"
#include "CommonMacros.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "TPSWeapon.h"
#include "PlayerInterface.h"
#include "HealthBarWidget.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ATPSCharacter::ATPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AssignDefaultSubobject(SpringArm)->SetupAttachment(GetRootComponent());
	AssignDefaultSubobject(Camera)->SetupAttachment(SpringArm, SpringArm->SocketName);

	SpringArm->bUsePawnControlRotation = true;
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->SetCapsuleHalfHeight(90);
	
	auto* const Movement = GetCharacterMovement();

	Movement->SetCrouchedHalfHeight(70);
	Movement->GetNavAgentPropertiesRef().bCanCrouch = true;
	Movement->JumpZVelocity = 500;
	Movement->MaxWalkSpeed = MaxSpeed;

	WeaponClasses.SetNum(3);
}

void ATPSCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	FActorSpawnParameters Parameters;
	Parameters.Owner = this;
	Parameters.Instigator = this;

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);

	for (auto& WeaponClass : WeaponClasses)
	{
		auto* NewWeapon = GetWorld()->SpawnActor<ATPSWeapon>(WeaponClass, Parameters);
		NewWeapon->AttachToComponent(GetMesh(), Rules, SocketOnWeaponAttached);
		NewWeapon->SetActorHiddenInGame(true);
		Weapons.Add(NewWeapon);
	}

	Weapon = Weapons[0];
	Weapon->SetActorHiddenInGame(false);
	
	if (Weapon->UserAnimClass != nullptr)
	{
		GetMesh()->LinkAnimClassLayers(Weapon->UserAnimClass);
	}

	CurrentHealth = MaxHealth;

	if (PlayerHUDClass != nullptr)
	{
		PlayerHUD = CreateWidget<UPlayerInterface>(GetWorld(), PlayerHUDClass);

		if (PlayerHUD != nullptr)
		{
			PlayerHUD->AddToViewport();

			if (PlayerHUD->PlayerHPBar != nullptr)
			{
				PlayerHUD->PlayerHPBar->SetBarColor(FLinearColor::White);
				PlayerHUD->PlayerHPBar->UpdateHP(CurrentHealth, MaxHealth);
			}
		}
	}
}

// Called every frame
void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Camera != nullptr)
	{
		float TargetFOV = bIsZooming ? AimFOV : DefaultFOV;
		float CurrentFOV = Camera->FieldOfView;

		float ZoomFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, ZoomInterpSpeed);

		Camera->SetFieldOfView(ZoomFOV);
	}
}

// Called to bind functionality to input
void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATPSCharacter::OnActionAStarted()
{
	Weapon->BeginActionA();
	GetWorldTimerManager().ClearTimer(FireStanceTimer);
	BeginAiming();
}

void ATPSCharacter::OnActionACompleted()
{
	Weapon->EndActionA();
	GetWorldTimerManager().SetTimer(FireStanceTimer, this, &ATPSCharacter::TryEndAimingStance, 1.0f, false);
}
void ATPSCharacter::OnActionBStarted()
{
	bIsActionBHeld = true;
	BeginAiming();
	bIsZooming = true;
}
void ATPSCharacter::OnActionBCompleted()
{
	bIsActionBHeld = false;
	if (!GetWorldTimerManager().IsTimerActive(FireStanceTimer))
	{
		EndAiming();
	}
	bIsZooming = false;
}

void ATPSCharacter::BeginAiming()
{
	bIsAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

void ATPSCharacter::EndAiming()
{
	bIsAiming = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void ATPSCharacter::TryEndAimingStance()
{
	if (!bIsActionBHeld)
	{
		EndAiming();
	}
}

void ATPSCharacter::EquipWeapon(int32 Index)
{
	ATPSWeapon* SelectedWeapon = Weapons[Index];

	if (Weapon == SelectedWeapon)
	{
		return;
	}

	if (Weapon != nullptr)
	{
		Weapon->CancelReload();
		Weapon->SetActorHiddenInGame(true);
	}

	if (Weapon->UserAnimClass != nullptr)
	{
		GetMesh()->UnlinkAnimClassLayers(Weapon->UserAnimClass);
	}

	Weapon = SelectedWeapon;
	Weapon->SetActorHiddenInGame(false);

	Weapon->SetOwner(this);

	Weapon->UpdateAmmoUI();
	
	if (Weapon->UserAnimClass != nullptr)
	{
		GetMesh()->LinkAnimClassLayers(Weapon->UserAnimClass);
	}

	PlayAnimMontage(Weapon->OnEquipped);
}

void ATPSCharacter::ReloadWeapon()
{
	PlayAnimMontage(Weapon->OnReload);
	Weapon->BeginSubAction();
}

void ATPSCharacter::PlayHitReactMontage(AActor* Attacker)
{
	if (!OnHit || !Attacker) return;

	FVector HitDirection = (Attacker->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	float ForwardDot = FVector::DotProduct(GetActorForwardVector(), HitDirection);
	float RightDot = FVector::DotProduct(GetActorRightVector(), HitDirection);

	FName SectionName;

	if (FMath::Abs(ForwardDot) >= FMath::Abs(RightDot))
	{
		if (ForwardDot > 0.f)
		{
			SectionName = TEXT("Front_Med");
		}
		else
		{
			SectionName = TEXT("Back_Med");
		}
	}
	else
	{
		if (RightDot > 0.f)
		{
			SectionName = TEXT("Right_Med");
		}
		else
		{
			SectionName = TEXT("Left_Med");
		}
	}

	PlayAnimMontage(OnHit, 1.0f, SectionName);
}

float ATPSCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHealth -= FinalDamage;

	if (PlayerHUD != nullptr && PlayerHUD->PlayerHPBar != nullptr)
	{
		PlayerHUD->PlayerHPBar->UpdateHP(CurrentHealth, MaxHealth);
	}
	if (CurrentHealth <= 0)
	{
		Die();
	}
	else 
	{
		PlayHitReactMontage(DamageCauser);
		ShowDamageUI();
	}

	return FinalDamage;
}

void ATPSCharacter::Die()
{
	bIsDead = true;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	SetLifeSpan(5.0f);
}




