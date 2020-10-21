// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerDungeonController.generated.h"

class APlayerCharacter;
class APlayerSpawn;

UCLASS()
class PROCGENWORLD_API APlayerDungeonController : public APawn
{
	GENERATED_BODY()

	APlayerSpawn* _playerSpawn;
	APlayerCharacter* _playerCharacter;

#pragma region Player Movement

	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();

#pragma endregion

	void HandlePlayerSpawnComplete(APlayerCharacter* playerCharacter);

public:
	// Sets default values for this pawn's properties
	APlayerDungeonController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
