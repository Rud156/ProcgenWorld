// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyControllerBase.h"
#include "ArcherController.generated.h"

/**
 *
 */
UCLASS()
class PROCGENWORLD_API AArcherController : public AEnemyControllerBase
{
	GENERATED_BODY()

	void FindTargetRowAndColumn(int& row, int& column);
	void GetRandomDirection(int& row, int& column);
	void Attack();

	bool IsPlayerInEnemyView();

public:
#pragma region Properties

	UPROPERTY(Category = EnemyStats, EditAnywhere)
		float MinAttackDistance;

	UPROPERTY(Category = EnemyStats, EditAnywhere)
		int DamageAmount;

#pragma endregion

	AArcherController();

	virtual void Execute() override;
};
