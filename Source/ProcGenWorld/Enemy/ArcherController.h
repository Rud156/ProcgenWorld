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
	void Attack();

public:
	virtual void Execute() override;
};
