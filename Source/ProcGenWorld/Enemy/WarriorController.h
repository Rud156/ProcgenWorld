// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyControllerBase.h"
#include "WarriorController.generated.h"

/**
 * 
 */
UCLASS()
class PROCGENWORLD_API AWarriorController : public AEnemyControllerBase
{
	GENERATED_BODY()

	void FindTargetRowAndColumn(int& row, int& column);
	bool IsPlayerInAdjacentTile();

	void Attack();

public:
	virtual void Execute() override;
};
