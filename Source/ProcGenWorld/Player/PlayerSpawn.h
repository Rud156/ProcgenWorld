// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerSpawn.generated.h"

class ADungeonGenerator;
class APlayerCharacter;
class APlayerModel;
class ADefaultPawn;
class ATriggerBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerSpawnComplete, APlayerCharacter*, PlayerCharacter);

UCLASS()
class PROCGENWORLD_API APlayerSpawn : public AActor
{
	GENERATED_BODY()

	ADungeonGenerator* _dungeonGen;

	APlayerModel* _playerModel;
	APlayerCharacter* _playerCharacter;
	ADefaultPawn* _defaultPawn;

	ATriggerBox* _victoryTrigger;


	UFUNCTION()
	void RoomGeneratonComplete();

	UFUNCTION()
	void RoomGenerationStarted();

	UFUNCTION()
	void SpawnPlayer();

public:

	UPROPERTY(Category = Player, EditAnywhere)
		TSubclassOf<class APlayerCharacter> PlayerCharacterPrefab;

	UPROPERTY(Category = Player, EditAnywhere)
		TSubclassOf<class APlayerModel> PlayerModelPrefab;

	UPROPERTY(Category = Spawn, EditAnywhere)
		TSubclassOf<class ATriggerBox> TriggerBoxPrefab;

	UPROPERTY(Category = Spawn, EditAnywhere)
		FVector SpawnOffset;

	UPROPERTY(Category = Spawn, EditAnywhere)
		float PlayerSpawnDelay;

	UPROPERTY(Category = TriggerCollision, EditAnywhere)
		FVector TriggerBoxSpawnOffset;

	UPROPERTY(Category = TriggerCollision, EditAnywhere)
		FVector TriggerBoxSpawnScale;

	UPROPERTY(Category = Spawning, BlueprintAssignable)
		FPlayerSpawnComplete OnPlayerSpawnComplete;

	// Sets default values for this actor's properties
	APlayerSpawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
