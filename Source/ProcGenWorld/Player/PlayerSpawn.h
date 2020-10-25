// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerSpawn.generated.h"

class ADungeonGenerator;
class APlayerCharacter;
class APlayerTopDownController;
class APlayerModel;
class ADefaultPawn;
class ATriggerBox;
class AGameController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerSpawnComplete, APlayerCharacter*, PlayerCharacter);

UCLASS()
class PROCGENWORLD_API APlayerSpawn : public AActor
{
	GENERATED_BODY()

	ADungeonGenerator* _dungeonGen;
	AGameController* _gameController;

	APlayerModel* _playerModel;
	APlayerCharacter* _playerCharacter;
	APlayerTopDownController* _playerTopDownController;
	ADefaultPawn* _defaultPawn;

	ATriggerBox* _victoryTrigger;


	UFUNCTION()
	void RoomGenerationComplete();

	UFUNCTION()
	void RoomGenerationStarted();

	UFUNCTION()
	void SpawnPlayer();

public:

	UPROPERTY(Category = Player, EditAnywhere)
		TSubclassOf<class APlayerCharacter> PlayerCharacterPrefab;

	UPROPERTY(Category = Player, EditAnywhere)
		TSubclassOf<class APlayerModel> PlayerModelPrefab;

	UPROPERTY(Category = Player, EditAnywhere)
		TSubclassOf<class APlayerTopDownController> PlayerTopDownControllerPrefab;

	UPROPERTY(Category = Spawn, EditAnywhere)
		TSubclassOf<class ATriggerBox> TriggerBoxPrefab;

	UPROPERTY(Category = Spawn, EditAnywhere)
		float PlayerSpawnDelay;

	UPROPERTY(Category = TriggerCollision, EditAnywhere)
		FVector TriggerBoxSpawnOffset;

	UPROPERTY(Category = TriggerCollision, EditAnywhere)
		FVector TriggerBoxSpawnScale;

	UPROPERTY(Category = Spawning, BlueprintAssignable)
		FPlayerSpawnComplete OnPlayerSpawnComplete;

	APlayerSpawn();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
};
