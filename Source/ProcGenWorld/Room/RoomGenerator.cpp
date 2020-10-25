// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomGenerator.h"
#include "Tile.h"
#include "../Player/PlayerTopDownController.h"
#include "../Enemy/EnemyControllerBase.h"

#include "Kismet/GameplayStatics.h"

#include "Math/UnrealMathUtility.h"
#include "Misc/OutputDeviceNull.h"

// Sets default values
ARoomGenerator::ARoomGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARoomGenerator::BeginPlay()
{
	Super::BeginPlay();

	_topRowDoorPosition = FVector::ZeroVector;
	_bottomRowDoorPosition = FVector::ZeroVector;
	_leftColumnDoorPosition = FVector::ZeroVector;
	_rightColumnDoorPosition = FVector::ZeroVector;

	_room = TMap<int, TMap<int, FWindowsPlatformTypes::TCHAR>>();
	_roomEnemies = TArray<AEnemyControllerBase*>();
	_floorMatrix = TMap<int, TMap<int, ATile*>>();

	_walls = TArray<AActor*>();
	_floorTiles = TArray<ATile*>();

	_isLerpActive = false;
}

void ARoomGenerator::Destroyed()
{
	for (int i = 0; i < _walls.Num(); i++) {
		_walls[i]->Destroy();
	}

	for (int i = 0; i < _floorTiles.Num(); i++) {
		_floorTiles[i]->Destroy();
	}
}

// Called every frame
void ARoomGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_isLerpActive) {
		auto targetLocation = FMath::Lerp(_lerpStartPosition, _lerpTargetPosition, _lerpAmount);
		this->SetActorLocation(targetLocation);

		_lerpAmount += GetWorld()->GetDeltaSeconds() * LerpSpeed;
		if (_lerpAmount >= 1) {
			this->SetActorLocation(_lerpTargetPosition);
			_isLerpActive = false;

			for (int i = 0; i < _walls.Num(); i++) {
				FOutputDeviceNull ar;
				FString command = FString::Printf(TEXT("SetWallImmobile"));
				_walls[i]->CallFunctionByNameWithArguments(*command, ar, NULL, true);
			}

			for (int i = 0; i < _floorTiles.Num(); i++) {
				FOutputDeviceNull ar;
				FString command = FString::Printf(TEXT("SetTileImmobile"));
				_floorTiles[i]->CallFunctionByNameWithArguments(*command, ar, NULL, true);
			}
		}
	}
}

void ARoomGenerator::LoadRoomFromFile(FString roomName, FVector startPosition)
{
	this->SetActorLocation(startPosition);

	FString fileName = roomName + ".txt";
	_roomName = roomName;

	FString directory = FPaths::ProjectContentDir() + "/Data";
	FString result;

	FString filePath = directory + "/" + fileName;

	IPlatformFile& fileManager = FPlatformFileManager::Get().GetPlatformFile();
	if (fileManager.FileExists(*filePath)) {
		FFileHelper::LoadFileToString(result, *filePath, FFileHelper::EHashOptions::None);

		RenderRoomFromString(result, startPosition);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "File Not Found");
	}
}

void ARoomGenerator::RenderRoomFromString(FString roomString, FVector startPosition)
{
	_startPosition = startPosition;

	GenerateRoomMatrix(roomString);
	RenderRoomEdges(startPosition);
}

void ARoomGenerator::GenerateRoomMatrix(FString roomString)
{
	int currentRowIndex = 0;
	int currentColumnIndex = 0;

	int maxColumns = 0;

	for (int i = 0; i < roomString.Len(); i++) {
		auto letter = roomString[i];

		if (!_room.Contains(currentRowIndex)) {
			_room.Add(currentRowIndex, TMap<int, FWindowsPlatformTypes::TCHAR>());
		}

		if (letter == '\r') {
			currentRowIndex += 1;
			currentColumnIndex = 0;

			i += 1;
		}
		else if (letter == '\n') {
			currentRowIndex += 1;
			currentColumnIndex = 0;
		}
		else {
			if (maxColumns < currentColumnIndex) {
				maxColumns = currentColumnIndex;
			}

			_room[currentRowIndex].Add(currentColumnIndex, letter);
			currentColumnIndex += 1;
		}
	}

	_rowCount = currentRowIndex;
	_columnCount = maxColumns;
}

void ARoomGenerator::RenderRoomEdges(FVector startPosition)
{
	FRotator rotation90 = FRotator(0, 90, 0);

	// Top Row
	FVector topPosition = startPosition;
	for (int i = 0; i <= _columnCount; i++) {
		if (_room[0].Contains(i)) {
			auto letter = _room[0][i];

			if (letter == '-') {
				AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &topPosition, &FRotator::ZeroRotator);
				wallInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				_walls.Add(wallInstance);

				topPosition.X += WallWidth;
			}
			else if (letter == 'X') {
				_topRowDoorPosition = topPosition;
				topPosition.X += WallWidth;
			}
		}
	}

	// Bottom Row
	FVector bottomPosition = FVector(startPosition.X, startPosition.Y + (_rowCount - 1) * WallWidth + WallThickness, startPosition.Z);
	for (int i = 0; i <= _columnCount; i++) {
		if (_room[_rowCount].Contains(i)) {
			auto letter = _room[_rowCount][i];

			if (letter == '-') {
				AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &bottomPosition, &FRotator::ZeroRotator);
				wallInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				_walls.Add(wallInstance);

				bottomPosition.X += WallWidth;
			}
			else if (letter == 'X') {
				_bottomRowDoorPosition = bottomPosition;
				bottomPosition.X += WallWidth;
			}
		}
	}

	// Left Row
	FVector leftPosition = FVector(startPosition.X + WallThickness * 1.15f, startPosition.Y + WallThickness, startPosition.Z);
	for (int i = 1; i < _rowCount; i++) {
		if (_room[i].Contains(0)) {
			auto letter = _room[i][0];

			if (letter == '|') {
				AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &leftPosition, &rotation90);
				wallInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				_walls.Add(wallInstance);

				leftPosition.Y += WallWidth;
			}
			else if (letter == 'X') {
				_leftColumnDoorPosition = leftPosition;
				leftPosition.Y += WallWidth;
			}
		}
	}

	// Right Row
	FVector rightPosition = FVector(startPosition.X + (_columnCount + 1) * WallWidth - WallThickness * 0.1f, startPosition.Y + WallThickness, startPosition.Z);
	for (int i = 1; i < _rowCount; i++) {
		if (_room[i].Contains(_columnCount)) {
			auto letter = _room[i][_columnCount];

			if (letter == '|') {
				AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &rightPosition, &rotation90);
				wallInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				_walls.Add(wallInstance);

				rightPosition.Y += WallWidth;
			}
			else if (letter == 'X') {
				_rightColumnDoorPosition = rightPosition;
				rightPosition.Y += WallWidth;
			}
		}
	}

	FVector currentPosition = _startPosition;

	for (int i = 0; i < _rowCount - 1; i++) {
		if (!_floorMatrix.Contains(i)) {
			_floorMatrix.Add(i, TMap<int, ATile*>());
		}

		for (int j = 0; j <= _columnCount; j++) {
			AActor* floorInstance = GetWorld()->SpawnActor(FloorPrefab, &currentPosition, &FRotator::ZeroRotator);
			floorInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

			ATile* tile = Cast<ATile>(floorInstance);
			if (tile != nullptr) {
				tile->SetTileParent(this);
				tile->SetPositionInRoom(i, j);

				_floorTiles.Add(tile);
				_floorMatrix[i].Add(j, tile);
			}

			currentPosition.X += WallWidth;
		}

		currentPosition.X = _startPosition.X;
		currentPosition.Y += WallWidth;
	}
}

void ARoomGenerator::HandleUnitDied(AEnemyControllerBase* enemy)
{
	for (int i = 0; i < _roomEnemies.Num(); i++)
	{
		if (_roomEnemies[i] == enemy)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Enemy Killed");
			_roomEnemies.RemoveAt(i);
			break;
		}
	}

	if (_roomEnemies.Num() <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Room Cleared");
		_isRoomCleared = true;
	}
}

ATile* ARoomGenerator::GetRandomTileInRoom(int& row, int& column)
{
	int randomRow = FMath::RandRange(0, _rowCount - 2);
	int randomColumn = FMath::RandRange(0, _columnCount);

	row = randomRow;
	column = randomColumn;

	ATile* randomTile = _floorMatrix[randomRow][randomColumn];
	return randomTile;
}

ATile* ARoomGenerator::GetTileAtPosition(int row, int column)
{
	ATile* tile = _floorMatrix[row][column];
	return  tile;
}

AEnemyControllerBase* ARoomGenerator::GetEnemyAtPosition(int row, int column)
{
	for (int i = 0; i < _roomEnemies.Num(); i++)
	{
		auto enemy = _roomEnemies[i];
		if (enemy->GetRow() == row && enemy->GetColumn() == column)
		{
			return enemy;
		}
	}

	return nullptr;
}

void ARoomGenerator::ClearAllTilesStatus()
{
	for (int i = 0; i < _floorTiles.Num(); i++) {
		_floorTiles[i]->ClearTileMoveableStatus(_roomDefaultMaterial);
	}
}

void ARoomGenerator::MarkValidSpots(int currentRow, int currentColumn)
{
	ClearAllTilesStatus();

	int leftColumn = currentColumn - 1;
	int rightColumn = currentColumn + 1;
	int topRow = currentRow - 1;
	int bottomRow = currentRow + 1;

	if (leftColumn < 0) {
		leftColumn = 0;
	}
	if (rightColumn > _columnCount) {
		rightColumn = _columnCount;
	}
	if (topRow < 0) {
		topRow = 0;
	}
	if (bottomRow > _rowCount - 2) {
		bottomRow = _rowCount - 2;
	}


	if (leftColumn != currentColumn) {
		_floorMatrix[currentRow][leftColumn]->MarkTileMoveable(TileMarkerMaterial);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Left Is Same");
	}
	if (rightColumn != currentColumn) {
		_floorMatrix[currentRow][rightColumn]->MarkTileMoveable(TileMarkerMaterial);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Right Is Same");
	}
	if (topRow != currentRow) {
		_floorMatrix[topRow][currentColumn]->MarkTileMoveable(TileMarkerMaterial);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Top Is Same");
	}
	if (bottomRow != currentRow) {
		_floorMatrix[bottomRow][currentColumn]->MarkTileMoveable(TileMarkerMaterial);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Bottom Is Same");
	}
}

TMap<int, TMap<int, WorldElementType>> ARoomGenerator::GetWorldState()
{
	TMap<int, TMap<int, WorldElementType>> worldState = TMap<int, TMap<int, WorldElementType>>();

	for (int i = 0; i < _rowCount - 1; i++)
	{
		if (!worldState.Contains(i)) {
			worldState.Add(i, TMap<int, WorldElementType>());
		}

		for (int j = 0; j <= _columnCount; j++)
		{
			ATile* tile = _floorMatrix[i][j];
			AEnemyControllerBase* enemy = GetEnemyAtPosition(i, j);

			int playerRow = _playerController->GetPlayerRow();
			int playerColumn = _playerController->GetPlayerColumn();

			if (i == playerRow && j == playerColumn)
			{
				worldState[i].Add(j, WorldElementType::Player);
			}
			else if (enemy != nullptr)
			{
				worldState[i].Add(j, WorldElementType::Enemy);
			}
			else if (tile != nullptr && tile->TileType == TileType::LavaTile)
			{
				worldState[i].Add(j, WorldElementType::LavaTile);
			}
			else
			{
				worldState[i].Add(j, WorldElementType::Floor);
			}
		}
	}

	return  worldState;
}

FString ARoomGenerator::GetRoomName()
{
	return _roomName;
}

FVector ARoomGenerator::GetStartPosition()
{
	return _startPosition;
}

void ARoomGenerator::UpdateRoomPosition(FVector offset)
{
	_lerpStartPosition = this->GetActorLocation();
	_lerpTargetPosition = _startPosition + offset;
	_lerpAmount = 0;
	_isLerpActive = true;

	_startPosition += offset;
	_topRowDoorPosition += offset;
	_bottomRowDoorPosition += offset;
	_leftColumnDoorPosition += offset;
	_rightColumnDoorPosition += offset;
}

FVector ARoomGenerator::GetTopRowDoorPosition()
{
	return _topRowDoorPosition;
}

FVector ARoomGenerator::GetBottomRowDoorPosition()
{
	return _bottomRowDoorPosition;
}

FVector ARoomGenerator::GetLeftColumnDoorPosition()
{
	return _leftColumnDoorPosition;
}

FVector ARoomGenerator::GetRightColumnDoorPosition()
{
	return _rightColumnDoorPosition;
}

int ARoomGenerator::GetRowCount()
{
	return _rowCount;
}

int ARoomGenerator::GetColumnCount()
{
	return _columnCount;
}

void ARoomGenerator::SetPlayerController(APlayerTopDownController* playerController)
{
	_playerController = playerController;
}

void ARoomGenerator::SpawnEnemies()
{
	_roomEnemies = TArray<AEnemyControllerBase*>();

	// TODO: Implement this function...
}

TArray<AEnemyControllerBase*> ARoomGenerator::GetEnemies()
{
	return  _roomEnemies;
}

bool ARoomGenerator::IsRoomCleared()
{
	return _isRoomCleared;
}

TArray<ATile*> ARoomGenerator::GetFloorTiles()
{
	return _floorTiles;
}

TArray<AActor*> ARoomGenerator::GetWalls()
{
	return _walls;
}

void ARoomGenerator::SetFloorColor(int roomType, UStaticMeshComponent* mesh)
{
	// 1: Spawn, 2: Exit, 3: General

	if (roomType == 1) {
		mesh->SetMaterial(0, SpawnMaterial);
		_roomDefaultMaterial = SpawnMaterial;
	}
	else if (roomType == 2) {
		mesh->SetMaterial(0, ExitMaterial);
		_roomDefaultMaterial = ExitMaterial;
	}
	else {
		mesh->SetMaterial(0, GeneralMaterial);
		_roomDefaultMaterial = GeneralMaterial;
	}
}
