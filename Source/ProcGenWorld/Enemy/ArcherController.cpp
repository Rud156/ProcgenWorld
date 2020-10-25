// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcherController.h"
#include "../Room/RoomGenerator.h"
#include "../Player/PlayerTopDownController.h"

AArcherController::AArcherController() : Super()
{
	MinAttackDistance = 2;
	DamageAmount = 1;
}

void AArcherController::Execute()
{
	bool isPlayerInView = IsPlayerInEnemyView();
	if (isPlayerInView)
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

void AArcherController::Attack()
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Archer Controller Attacked Player");
	_playerTopDownController->TakeDamage(DamageAmount);
}


void AArcherController::FindTargetRowAndColumn(int& row, int& column)
{
	int playerRow = _playerTopDownController->GetPlayerRow();
	int playerColumn = _playerTopDownController->GetPlayerColumn();

	int rowDiff = FMath::Abs(_currentRow - playerRow);
	int colDiff = FMath::Abs(_currentColumn - playerColumn);

	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::SanitizeFloat(colDiff));

	int floorRows = _parentRoom->GetRowCount() - 2;
	int floorColumns = _parentRoom->GetColumnCount();

	if (_currentRow == playerRow && colDiff < MinAttackDistance)
	{
		int difference = _currentColumn - playerColumn;

		// Not Possible to Have Difference 0
		if (difference < 0)
		{
			int targetRow = _currentRow;
			int targetColumn = _currentColumn - 1;

			if (targetColumn < 0)
			{
				GetRandomDirection(targetRow, targetColumn);
			}

			row = targetRow;
			column = targetColumn;
		}
		else if (difference > 0)
		{
			int targetRow = _currentRow;
			int targetColumn = _currentColumn + 1;

			if (targetColumn > floorColumns)
			{
				GetRandomDirection(targetRow, targetColumn);
			}

			row = targetRow;
			column = targetColumn;
		}
	}
	else if (_currentColumn == playerColumn && rowDiff < MinAttackDistance)
	{
		int difference = _currentRow - playerRow;

		// Not Possible to Have Difference 0
		if (difference < 0)
		{
			int targetRow = _currentRow - 1;
			int targetColumn = _currentColumn;

			if (targetRow < 0)
			{
				GetRandomDirection(targetRow, targetColumn);
			}
			else if (targetRow > floorRows)
			{
				GetRandomDirection(targetRow, targetColumn);
			}

			row = targetRow;
			column = targetColumn;
		}
		else if (difference > 0)
		{
			int targetRow = _currentRow + 1;
			int targetColumn = _currentColumn;

			if (targetRow > floorRows)
			{
				GetRandomDirection(targetRow, targetColumn);
			}

			row = targetRow;
			column = targetColumn;
		}
	}
	else
	{
		if (_currentRow < playerRow)
		{
			row = _currentRow + 1;
			column = _currentColumn;
		}
		else if (_currentRow > playerRow)
		{
			row = _currentRow - 1;
			column = _currentColumn;
		}
		else if (_currentColumn < playerColumn)
		{
			row = _currentRow;
			column = _currentColumn + 1;
		}
		else if (_currentColumn > playerColumn)
		{
			row = _currentRow;
			column = _currentColumn - 1;
		}
	}

}

void AArcherController::GetRandomDirection(int& row, int& column)
{
	int floorRows = _parentRoom->GetRowCount() - 2;
	int floorColumns = _parentRoom->GetColumnCount();

	TArray<int> randomArray = { 0, 0, 0, 0 };

	// Left
	if (_currentColumn - 1 >= 0) {
		randomArray[0] = 1;
	}

	// Right
	if (_currentColumn + 1 >= floorColumns) {
		randomArray[1] = 1;
	}

	// Top
	if (_currentRow - 1 >= 0) {
		randomArray[2] = 1;
	}

	// Bottom
	if (_currentRow + 1 <= floorRows) {
		randomArray[3] = 1;
	}

	TArray<int> validIndex;
	for (int i = 0; i < randomArray.Num(); i++) {
		if (randomArray[i] == 1) {
			validIndex.Add(i);
		}
	}

	if (validIndex.Num() <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Invalid Array Size 0!!!");
	}

	int randomIndex = FMath::RandRange(0, validIndex.Num() - 1);
	int randomSide = validIndex[randomIndex];

	if (randomSide == 0) {
		row = _currentRow;
		column = _currentColumn - 1;
	}
	else if (randomSide == 1) {
		row = _currentRow;
		column = _currentColumn + 1;
	}
	else if (randomSide == 2) {
		row = _currentRow - 1;
		column = _currentColumn;
	}
	else {
		row = _currentRow + 1;
		column = _currentColumn;
	}
}

bool AArcherController::IsPlayerInEnemyView()
{
	auto worldState = _parentRoom->GetWorldState();
	int floorRows = _parentRoom->GetRowCount() - 2;
	int floorColumns = _parentRoom->GetColumnCount();

	// Left Side
	for (int i = _currentColumn - 1; i >= 0; i--)
	{
		int diff = FMath::Abs(i - _currentColumn);
		if (worldState[_currentRow][i] == WorldElementType::Player && diff >= MinAttackDistance)
		{
			return true;
		}
		else if (worldState[_currentRow][i] == WorldElementType::Enemy)
		{
			break;
		}
	}

	// Right Side
	for (int i = _currentColumn + 1; i <= floorColumns; i++)
	{
		int diff = FMath::Abs(i - _currentColumn);
		if (worldState[_currentRow][i] == WorldElementType::Player && diff >= MinAttackDistance)
		{
			return true;
		}
		else if (worldState[_currentRow][i] == WorldElementType::Enemy)
		{
			break;
		}
	}

	// Top Side
	for (int i = _currentRow - 1; i >= 0; i--)
	{
		int diff = FMath::Abs(i - _currentRow);
		if (worldState[i][_currentColumn] == WorldElementType::Player && diff >= MinAttackDistance)
		{
			return true;
		}
		else if (worldState[i][_currentColumn] == WorldElementType::Enemy)
		{
			break;
		}
	}

	// Bottom Side
	for (int i = _currentRow + 1; i <= floorRows; i++)
	{
		int diff = FMath::Abs(i - _currentRow);
		if (worldState[i][_currentColumn] == WorldElementType::Player && diff >= MinAttackDistance)
		{
			return true;
		}
		else if (worldState[i][_currentColumn] == WorldElementType::Enemy)
		{
			break;
		}
	}

	return false;
}
