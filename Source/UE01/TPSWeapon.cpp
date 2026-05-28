// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSWeapon.h"
#include "CommonMacros.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFrameWork/Character.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "PlayerInterface.h"
#include "TPSCharacter.h"

//#define DRAW_DEBUG

// Sets default values
ATPSWeapon::ATPSWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(AssignDefaultSubobject(Mesh));
}

// Called when the game starts or when spawned
void ATPSWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	CashedOwner = Cast<ATPSCharacter>(GetOwner());

	CurrentAmmo = MaxAmmo;
	UpdateAmmoUI();
}

// Called every frame
void ATPSWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATPSWeapon::BeginFire()
{
	if (CurrentAmmo <= 0) {
		BeginReload();
		return;
	}

	Fire();

	// 단사, 연사 구분
	if (bIsAutomatic) 
	{
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ATPSWeapon::Fire, FireRate, true);
	}
}

void ATPSWeapon::EndFire()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	CurrentSpread = BaseSpread;
}

void ATPSWeapon::Fire()
{
	if (bIsReloading)
	{
		return;
	}

	if (CurrentAmmo <= 0)
	{
		BeginReload();

		return;
	}

	if (CashedOwner == nullptr)
	{ 
		return;
	}

	APlayerController* OwnerController = Cast<APlayerController>(CashedOwner->GetController());
	if (OwnerController == nullptr)
	{
		return;
	}

	// 애니메이션 재생
	if (OnFire != nullptr)
	{
		CashedOwner->PlayAnimMontage(OnFire);
	}

	// 반동 효과
	if (FireCameraShakeClass != nullptr)
	{
		OwnerController->ClientStartCameraShake(FireCameraShakeClass);
	}

	OwnerController->AddPitchInput(-RecoilForce);
	OwnerController->AddYawInput(FMath::RandRange(-RecoilForce * 0.5f, RecoilForce * 0.5f));

	// 총구 화염 및 탄피
	FVector MuzzleLocation = Mesh->GetSocketLocation(MuzzleSocket);
	FVector MuzzleForward = Mesh->GetSocketQuaternion(MuzzleSocket).GetForwardVector();

	FFXSystemSpawnParameters SpawnParams
	{
		.WorldContextObject = GetWorld(),
		.SystemTemplate = MuzzleFlashSystem,
		.AttachToComponent = Mesh,
		.AttachPointName = MuzzleSocket,
		.LocationType = EAttachLocation::Type::KeepRelativeOffset
	};

	auto* MuzzleFlash = UNiagaraFunctionLibrary::SpawnSystemAttachedWithParams(SpawnParams);
	MuzzleFlash->SetBoolParameter("Trigger", true);
	MuzzleFlash->SetVectorParameter("Direction", MuzzleForward);

	SpawnParams.SystemTemplate = ShellEjectSystem;
	SpawnParams.AttachPointName = ShellEjectSocket;
	auto ShellEject = UNiagaraFunctionLibrary::SpawnSystemAttachedWithParams(SpawnParams);
	ShellEject->SetBoolParameter("Trigger", true);
	ShellEject->SetVariableStaticMesh("ShellEjectStaticMesh", ShellEjectMesh);

	// 히트 스캔
	FVector CameraLocation;
	FRotator CameraRotation;

	OwnerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceEndPoint;

	SpawnParams.SystemTemplate = TracerSystem;
	SpawnParams.AttachPointName = MuzzleSocket;

	// 탄 퍼짐 효과
	for (int32 i = 0; i < PelletCount; i++)
	{
		CurrentSpread += SpreadIncreasePerShot;
		CurrentSpread = FMath::Min(CurrentSpread, MaxSpread);

		float HalfRadian = FMath::DegreesToRadians(CurrentSpread);

		FVector ShootDirection = FMath::VRandCone(CameraRotation.Vector(), HalfRadian);

		FVector StartPoint = CameraLocation;
		FVector EndPoint = StartPoint + ShootDirection * MaxRange;

		// 플레이어 또는 자기자신(무기) 무시
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;

		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(CashedOwner);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			StartPoint,
			EndPoint,
			ECC_Visibility,
			QueryParams
		);
#if defined DRAW_DEBUG
		if (bHit)
		{
			DrawDebugLine(GetWorld(), StartPoint, HitResult.ImpactPoint, FColor::Red, false, 5.0f);
			DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f,FColor::Red, false, 5.0f);
		}
		else
		{
			DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Green, false, 5.0f);
		}
#endif
		if (bHit)
		{
			TraceEndPoint = HitResult.ImpactPoint;

			AActor* HitActor = HitResult.GetActor();
			if (HitActor != nullptr)
			{
				UGameplayStatics::ApplyPointDamage(
					HitActor,
					BaseDamage,
					StartPoint - EndPoint,
					HitResult,
					OwnerController,
					this,
					UDamageType::StaticClass()
				);
			}

			if (BulletDecalMaterial != nullptr)
			{
				UDecalComponent* SpawnedDecal = UGameplayStatics::SpawnDecalAttached(
					BulletDecalMaterial,
					FVector(5.0f, 5.0f, 5.0f),
					HitResult.GetComponent(),
					HitResult.BoneName,
					HitResult.ImpactPoint,
					HitResult.ImpactNormal.Rotation(),
					EAttachLocation::KeepWorldPosition,
					10.0f
				);

				if (SpawnedDecal != nullptr)
				{
					SpawnedDecal->SetFadeScreenSize(0.001f);
				}
			}
		}
		else
		{
			TraceEndPoint = EndPoint;
		}
 
		// 총알 궤적
		auto Tracer = UNiagaraFunctionLibrary::SpawnSystemAttachedWithParams(SpawnParams);
		Tracer->SetBoolParameter("Trigger", true);
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector
		(
			Tracer,
			"ImpactPositions",
			{ TraceEndPoint }
		);
	}

	CurrentAmmo--;

	UpdateAmmoUI();

	if (CurrentAmmo <= 0)
	{
		BeginReload();
	}
}
void ATPSWeapon::BeginReload()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);

	if (CurrentAmmo == MaxAmmo || bIsReloading)
	{
		return;
	}
	
	bIsReloading = true;

	if (CashedOwner != nullptr && OnReload != nullptr)
	{
		CashedOwner->PlayAnimMontage(OnReload);
	}
}

void ATPSWeapon::EndReload()
{
	CurrentAmmo = MaxAmmo;
	UpdateAmmoUI();
	bIsReloading = false;
}

void ATPSWeapon::UpdateAmmoUI()
{
	if (CashedOwner != nullptr && CashedOwner->PlayerHUD != nullptr)
	{
		CashedOwner->PlayerHUD->UpdateAmmo(CurrentAmmo, MaxAmmo);
	}
}

void ATPSWeapon::CancelReload()
{
	if (bIsReloading)
	{
		bIsReloading = false;
	}
}

