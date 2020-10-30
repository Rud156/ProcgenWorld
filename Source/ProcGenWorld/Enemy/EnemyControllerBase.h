// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyControllerBase.generated.h"

class ARoomGenerator;
class APlayerTopDownController;
class ATile;

UCLASS()
class PROCGENWORLD_API AEnemyControllerBase : public ACharacter
{
	GENERATED_BODY()

	ATile* _lastTargetTile;

public:
	UPROPERTY(Category = EnemyStats, EditAnywhere)
		float MaxHealth;

	AEnemyControllerBase();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Category = Movement, BlueprintImplementableEvent)
		void MoveEnemyTo(FVector targetLocation);

	UFUNCTION(Category = AI, BlueprintCallable)
		void HandleEnemyReachedPosition();

	void SetParentRoom(ARoomGenerator* roomGenerator);
	void SetSpawnPosition(int row, int column);
	int GetRow();
	int GetColumn();

	int GetCurrentHealth();
	int GetMaxHealth();
	void TakeDamage(int damageAmount);
	void HandleUnitDied();

	virtual void Execute();
	void Move(int row, int column);

protected:
#pragma region Properties

	ARoomGenerator* _parentRoom;
	APlayerTopDownController* _playerTopDownController;

	float _health;

	int _currentRow;
	int _currentColumn;

#pragma endregion

	virtual void BeginPlay() override;
};
