// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomGenerator.h"

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

	_walls = TArray<AActor*>();
	_floorTiles = TArray<AActor*>();

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
		auto targetLocation = FMath::Lerp(_lerpStartPositon, _lerpTargetPosition, _lerpAmount);
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
		for (int j = 0; j <= _columnCount; j++) {
			AActor* floorInstance = GetWorld()->SpawnActor(FloorPrefab, &currentPosition, &FRotator::ZeroRotator);
			floorInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			_floorTiles.Add(floorInstance);

			currentPosition.X += WallWidth;
		}

		currentPosition.X = _startPosition.X;
		currentPosition.Y += WallWidth;
	}
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
	_lerpStartPositon = this->GetActorLocation();
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

TArray<AActor*> ARoomGenerator::GetFloorTiles()
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
	}
	else if (roomType == 2) {
		mesh->SetMaterial(0, ExitMaterial);
	}
	else {
		mesh->SetMaterial(0, GeneralMaterial);
	}
}
