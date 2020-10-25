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

#pragma region Structs

	struct QItem
	{
		int row;
		int column;
		int distance;

		QItem* parent;

		QItem(int x, int y, int d, QItem *p)
		{
			row = x;
			column = y;
			distance = d;
			parent = p;
		}
	};

#pragma endregion

public:

	UPROPERTY(Category = EnemyStats, EditAnywhere)
		int DamageAmount;

	AWarriorController();
	
	virtual void Execute() override;
};
