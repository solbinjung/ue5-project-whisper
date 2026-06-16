// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSEnemySpawner.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "TPSEnemy.h"

// Sets default values
ATPSEnemySpawner::ATPSEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));

	bHasTriggered = false;
}

// Called when the game starts or when spawned
void ATPSEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATPSEnemySpawner::OnTriggerOverlap);
}

void ATPSEnemySpawner::OnTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasTriggered || !EnemyClassToSpawn) return;

	ACharacter* Player = Cast<ACharacter>(OtherActor);
	if (Player)
	{
		bHasTriggered = true;

		for (const FVector& SpawnOffset : SpawnLocations)
		{
			FVector SpawnWorldLocation = GetActorTransform().TransformPosition(SpawnOffset);
			FRotator SpawnRotation = GetActorRotation();

			GetWorld()->SpawnActor<ATPSEnemy>(EnemyClassToSpawn, SpawnWorldLocation, SpawnRotation);
		}
	}
}
