// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomGenerator.h"
#include "Math/UnrealMathUtility.h"

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
	_floor = nullptr;

	_isLerpActive = false;

	//LoadRoomFromFile("C_B", FVector(0, 0, 800));
}

void ARoomGenerator::Destroyed()
{
	for (int i = 0; i < _walls.Num(); i++) {
		_walls[i]->Destroy();
	}

	if (_floor != nullptr) {
		_floor->Destroy();
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
	//RenderOtherRoomParts(startPosition);
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

			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Column: " + FString::SanitizeFloat(currentColumnIndex) + ", Letter: " + letter);
			_room[currentRowIndex].Add(currentColumnIndex, letter);
			currentColumnIndex += 1;
		}
	}

	_rowCount = currentRowIndex;
	_columnCount = maxColumns;

	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Row: " + FString::SanitizeFloat(_rowCount));
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Column: " + FString::SanitizeFloat(_columnCount));
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
			else if (letter == '|') {
				AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &topPosition, &rotation90);
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
			else if (letter == '|') {
				AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &bottomPosition, &rotation90);
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


	FRotator rotation45 = FRotator(0, 45, 0);
	FRotator rotation135 = FRotator(0, 135, 0);

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
			else if (letter == '-') {
				AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &leftPosition, &FRotator::ZeroRotator);
				wallInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				_walls.Add(wallInstance);

				leftPosition.Y += WallWidth;
			}
			else if (letter == '\\') {
				AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &leftPosition, &rotation45);
				wallInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				_walls.Add(wallInstance);

				leftPosition.Y += WallWidth;
			}
			else if (letter == '/') {
				AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &leftPosition, &rotation135);
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
			else if (letter == '-') {
				AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &rightPosition, &FRotator::ZeroRotator);
				wallInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				_walls.Add(wallInstance);

				rightPosition.Y += WallWidth;
			}
			else if (letter == '\\') {
				AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &rightPosition, &rotation45);
				wallInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				_walls.Add(wallInstance);

				rightPosition.Y += WallWidth;
			}
			else if (letter == '/') {
				AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &rightPosition, &rotation135);
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

	AActor* floorInstance = GetWorld()->SpawnActor(FloorPrefab, &_startPosition, &FRotator::ZeroRotator);
	floorInstance->SetActorScale3D(FVector(_rowCount + 1, _columnCount - 1, 1));
	floorInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	_floor = floorInstance;
}

void ARoomGenerator::RenderOtherRoomParts(FVector startPosition)
{
	FVector currentPosition = startPosition;
	currentPosition.X += WallThickness;
	currentPosition.Y += WallThickness;

	FRotator rotation90 = FRotator(0, 90, 0);
	FRotator rotation45 = FRotator(0, 45, 0);
	FRotator rotation135 = FRotator(0, 135, 0);

	for (int i = 1; i < _rowCount; i++) {
		for (int j = 1; j < _columnCount; j++) {
			if (_room[i].Contains(j)) {

				auto letter = _room[i][j];

				if (letter == '/') {
					AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &currentPosition, &rotation135);
					wallInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
					_walls.Add(wallInstance);
				}
				else if (letter == '\\') {
					AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &currentPosition, &rotation45);
					wallInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
					_walls.Add(wallInstance);
				}
				else if (letter == '-') {
					AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &currentPosition, &FRotator::ZeroRotator);
					wallInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
					_walls.Add(wallInstance);
				}
				else if (letter == '|') {
					AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &currentPosition, &rotation90);
					wallInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
					_walls.Add(wallInstance);
				}

				currentPosition.X += WallWidth;
			}
		}

		currentPosition.X = startPosition.X + WallThickness;
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

AActor* ARoomGenerator::GetFloor()
{
	return _floor;
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
