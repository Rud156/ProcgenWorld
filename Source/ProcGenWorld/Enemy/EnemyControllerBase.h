// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyControllerBase.generated.h"

class ARoomGenerator;
class APlayerTopDownController;

UCLASS()
class PROCGENWORLD_API AEnemyControllerBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyControllerBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetParentRoom(ARoomGenerator* roomGenerator);
	void SetSpawnPosition(int row, int column);

	void TakeDamage(int damageAmount);
	void HandleUnitDied();

	virtual void Execute();

protected:
#pragma region Properties

	ARoomGenerator* _parentRoom;
	APlayerTopDownController* _playerTopDownController;

	float _health;

	int _currentRow;
	int _currentColumn;

#pragma endregion

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(int row, int column);
};
