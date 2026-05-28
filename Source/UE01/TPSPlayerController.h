// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPSPlayerController.generated.h"

class ATPSCharacter;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class UE01_API ATPSPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	void OnMoveTriggered(const FInputActionValue& Value);
	void OnLookTriggered(const FInputActionValue& Value);
	void OnActionAStarted();
	void OnActionACompleted();
	void OnActionBStarted();
	void OnActionBCompleted();
	void OnCrouchStarted();
	void OnCrouchCompleted();
	void OnJumpStarted();
	void OnSelectRifleStarted();
	void OnSelectPistolStarted();
	void OnSelectShotgunStarted();
	void OnReloadStarted();
	
protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<ATPSCharacter> TPSCharacter;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction_Move;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction_Look;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction_ActionA;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction_ActionB;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction_Crouch;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction_Jump;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction_SelectRifle;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction_SelectPistol;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction_SelectShotgun;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction_Reload;
};
