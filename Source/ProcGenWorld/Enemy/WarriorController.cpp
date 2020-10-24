// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorController.h"

void AWarriorController::Execute()
{
	bool isPlayerAdjacent = IsPlayerInAdjacentTile();

	if (isPlayerAdjacent)
	{
		Attack();
	}
	else
	{
		int targetRow;
		int targetColumn;

		FindTargetRowAndColumn(targetRow, targetColumn);
		Move(targetRow, targetColumn);
	}
}

void AWarriorController::Attack()
{
}


void AWarriorController::FindTargetRowAndColumn(int& row, int& column)
{
}

bool AWarriorController::IsPlayerInAdjacentTile()
{
	return false;
}
