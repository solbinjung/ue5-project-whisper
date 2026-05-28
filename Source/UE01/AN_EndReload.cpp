// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_EndReload.h"
#include "TPSCharacter.h"
#include "TPSWeapon.h"

void UAN_EndReload::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		ATPSCharacter* OwnerCharacter = Cast<ATPSCharacter>(MeshComp->GetOwner());

		if (OwnerCharacter != nullptr)
		{
			if (OwnerCharacter->Weapon != nullptr)
			{
				OwnerCharacter->Weapon->EndReload();
			}
		}
	}
}
