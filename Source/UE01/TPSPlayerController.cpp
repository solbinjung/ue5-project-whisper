// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "TPSCharacter.h"

void ATPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	auto* LocalPlayer = Cast<ULocalPlayer>(Player);
	check(LocalPlayer);

	auto* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	check(InputMappingContext);
	Subsystem->AddMappingContext(InputMappingContext, InputPriority);

	TArray<FEnhancedActionKeyMapping> Mappings = InputMappingContext->GetMappings();

	auto* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	check(EnhancedInputComponent);

#define Bind(Action, Event)\
BindAction(InputAction_##Action, ETriggerEvent::Event, this, &ThisClass::On##Action##Event)
	EnhancedInputComponent->Bind(Move, Triggered);
	EnhancedInputComponent->Bind(Look, Triggered);
	EnhancedInputComponent->Bind(ActionA, Started);
	EnhancedInputComponent->Bind(ActionA, Completed);
	EnhancedInputComponent->Bind(ActionB, Started);
	EnhancedInputComponent->Bind(ActionB, Completed);
	EnhancedInputComponent->Bind(Crouch, Started);
	EnhancedInputComponent->Bind(Crouch, Completed);
	EnhancedInputComponent->Bind(Jump, Started);
	EnhancedInputComponent->Bind(SelectRifle, Started);
	EnhancedInputComponent->Bind(SelectPistol, Started);
	EnhancedInputComponent->Bind(SelectShotgun, Started);
	EnhancedInputComponent->Bind(Reload, Started);

#undef Bind

}
void ATPSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	TPSCharacter = Cast<ATPSCharacter>(InPawn);
	check(TPSCharacter);
}
void ATPSPlayerController::OnMoveTriggered(const FInputActionValue& Value)
{
	auto WorldDirection = Value.Get<FVector>();
	auto Rotation = FRotator(0, GetControlRotation().Yaw, 0);
	auto MoveDirection = Rotation.RotateVector(WorldDirection);
	TPSCharacter->AddMovementInput(MoveDirection);
}
void ATPSPlayerController::OnLookTriggered(const FInputActionValue& Value)
{
	auto MouseDelta = Value.Get<FVector2D>();
	AddYawInput(MouseDelta.X);
	AddPitchInput(MouseDelta.Y);
}

void ATPSPlayerController::OnActionAStarted()
{
	TPSCharacter->OnActionAStarted();
}

void ATPSPlayerController::OnActionACompleted()
{
	TPSCharacter->OnActionACompleted();

}

void ATPSPlayerController::OnActionBStarted()
{
	TPSCharacter->OnActionBStarted();
}

void ATPSPlayerController::OnActionBCompleted()
{
	TPSCharacter->OnActionBCompleted();
}

void ATPSPlayerController::OnCrouchStarted()
{
	TPSCharacter->Crouch();
}

void ATPSPlayerController::OnCrouchCompleted()
{
	TPSCharacter->UnCrouch();
}

void ATPSPlayerController::OnJumpStarted()
{
	TPSCharacter->Jump();
}

void ATPSPlayerController::OnSelectRifleStarted()
{
	TPSCharacter->EquipWeapon(0);
}

void ATPSPlayerController::OnSelectPistolStarted()
{
	TPSCharacter->EquipWeapon(1);
}

void ATPSPlayerController::OnSelectShotgunStarted()
{
	TPSCharacter->EquipWeapon(2);
}

void ATPSPlayerController::OnReloadStarted()
{
	TPSCharacter->ReloadWeapon();
}
