// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorController.h"
#include "../Room/RoomGenerator.h"
#include "../Player/PlayerTopDownController.h"
#include "../Data/EnumData.h"

AWarriorController::AWarriorController() : Super()
{
	DamageAmount = 1;
}


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
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Warrior Controller Attacked Player");
	_playerTopDownController->TakeDamage(DamageAmount);
}


void AWarriorController::FindTargetRowAndColumn(int& row, int& column)
{
	auto worldState = _parentRoom->GetWorldState();

	int floorRows = _parentRoom->GetRowCount() - 2;
	int floorColumns = _parentRoom->GetColumnCount();

	QItem* source = new QItem(0, 0, 0, nullptr);

	// Visited Matrix
	TMap<int, TMap<int, bool>> visited = TMap<int, TMap<int, bool>>();
	for (int i = 0; i <= floorRows; i++)
	{
		if (!visited.Contains(i)) {
			visited.Add(i, TMap<int, bool>());
		}

		for (int j = 0; j <= floorColumns; j++)
		{
			auto worldElement = worldState[i][j];
			switch (worldElement)
			{
			case WorldElementType::Player:
			{
				visited[i].Add(j, false);
			}
			break;

			case WorldElementType::Enemy:
			{
				if (i != _currentRow && j != _currentColumn)
				{
					visited[i].Add(j, true);
				}
				else
				{
					source->row = i;
					source->column = j;

					visited[i].Add(j, false);
				}
			}
			break;

			case WorldElementType::LavaTile:
				visited[i].Add(j, true);
				break;

			case WorldElementType::Floor:
				visited[i].Add(j, false);
				break;

			default:
				break;
			}
		}
	}

	TArray<QItem*> queue;
	queue.Add(source);
	visited[source->row].Add(source->column, false);

	int distance = -1; // Final Distance to be used
	QItem* path = nullptr;

	while (queue.Num() > 0)
	{
		auto item = queue[0];
		queue.RemoveAt(0);

		// Player Found
		if (worldState[item->row][item->column] == WorldElementType::Player)
		{
			distance = item->distance;
			path = item;
			break;
		}

		// Moving Up
		if (item->row - 1 >= 0 && !visited[item->row - 1][item->column])
		{
			queue.Add(new QItem(item->row - 1, item->column, item->distance + 1, item));
			visited[item->row - 1].Add(item->column, true);
		}

		// Moving Down
		if (item->row + 1 <= floorRows && !visited[item->row + 1][item->column])
		{
			queue.Add(new QItem(item->row + 1, item->column, item->distance + 1, item));
			visited[item->row + 1].Add(item->column, true);
		}

		// Moving Left
		if (item->column - 1 >= 0 && !visited[item->row][item->column - 1])
		{
			queue.Add(new QItem(item->row, item->column - 1, item->distance + 1, item));
			visited[item->row].Add(item->column - 1, true);
		}

		// Moving Right
		if (item->column + 1 <= floorColumns && !visited[item->row][item->column + 1])
		{
			queue.Add(new QItem(item->row, item->column + 1, item->distance + 1, item));
			visited[item->row].Add(item->column + 1, true);
		}
	}

	// Move in Random Direction
	if (distance == -1)
	{
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
	else
	{
		while (path->parent->parent != nullptr)
		{
			path = path->parent;
		}

		row = path->row;
		column = path->column;
	}
}

bool AWarriorController::IsPlayerInAdjacentTile()
{
	int leftSide = _currentColumn - 1;
	int rightSide = _currentColumn + 1;
	int topSide = _currentRow - 1;
	int bottomSide = _currentRow + 1;

	int playerRow = _playerTopDownController->GetPlayerRow();
	int playerColumn = _playerTopDownController->GetPlayerColumn();

	if (_currentRow == playerRow && leftSide == playerColumn)
	{
		return true;
	}

	if (_currentRow == playerRow && rightSide == playerColumn)
	{
		return true;
	}

	if (topSide == playerRow && _currentColumn == playerColumn)
	{
		return true;
	}

	if (bottomSide == playerRow && _currentColumn == playerColumn)
	{
		return true;
	}

	return false;
}
