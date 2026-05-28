// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TPSWeaponInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTPSWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UE01_API ITPSWeaponInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void BeginActionA() PURE_VIRTUAL(ITPSWeaponInterface::BeginActionA);
	virtual void EndActionA() PURE_VIRTUAL(ITPSWeaponInterface::EndActionA);
	virtual void BeginSubAction() PURE_VIRTUAL(ITPSWeaponInterface::BeginSubAction);
	virtual void EndSubAction() PURE_VIRTUAL(ITPSWeaponInterface::EndSubAction);
};
