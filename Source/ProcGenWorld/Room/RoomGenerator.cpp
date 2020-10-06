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

	_walls = TArray<AActor*>();
	_isLerpActive = false;
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

	FVector currentPosition = startPosition;
	FRotator rotation90 = FRotator(0, 90, 0);

	bool lastSpawnVertical = false;

	int rowCount = 0;
	int columnCount = 0;
	bool rowCountCalculated = false;

	bool topDoorPositionCalculated = false;
	bool bottomDoorPositionCalculated = false;
	bool leftDoorPositionCalculated = false;
	bool rightDoorPositionCalculated = false;


	int stringIndex = 0;
	while (stringIndex < roomString.Len())
	{
		auto letter = roomString[stringIndex];

		if (letter == '-')
		{
			AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &currentPosition, &FRotator::ZeroRotator);
			wallInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			_walls.Add(wallInstance);

			currentPosition.X += WallWidth;
			lastSpawnVertical = false;
		}
		else if (letter == '|')
		{
			auto nextLetter = roomString[stringIndex + 1];

			if (nextLetter != ' ') {
				currentPosition.X += WallWidth - WallThickness * 1.25f;
			}

			AActor* wallInstance = GetWorld()->SpawnActor(WallPrefab, &currentPosition, &rotation90);
			wallInstance->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			_walls.Add(wallInstance);

			currentPosition.X += WallWidth;
			lastSpawnVertical = true;
		}
		else if (letter == '\n')
		{
			auto nextLetter = roomString[stringIndex + 1];
			if (nextLetter == '-') {
				leftDoorPositionCalculated = true;
				rightDoorPositionCalculated = true;

				currentPosition.X = startPosition.X;
			}
			else {
				currentPosition.X = startPosition.X + WallThickness * 1.125f;
			}

			if (!lastSpawnVertical) {
				currentPosition.Y += WallThickness;
			}
			else {
				currentPosition.Y += WallWidth;
			}



			columnCount += 1;
			rowCountCalculated = true;
			topDoorPositionCalculated = true;
		}
		else if (letter == 'X') {
			auto nextLetter = ' ';
			if (stringIndex + 1 < roomString.Len()) {
				nextLetter = roomString[stringIndex + 1];
			}

			if (!topDoorPositionCalculated) {
				_topRowDoorPosition = currentPosition;
			}
			else if (!leftDoorPositionCalculated || !rightDoorPositionCalculated) {
				if (nextLetter == ' ') {
					_leftColumnDoorPosition = currentPosition;
					leftDoorPositionCalculated = true;
				}
				else {
					_rightColumnDoorPosition = currentPosition;
					_rightColumnDoorPosition.X += WallWidth - WallThickness * 1.25f;
					rightDoorPositionCalculated = true;
				}
			}
			else if (!bottomDoorPositionCalculated) {
				_bottomRowDoorPosition = currentPosition;
				bottomDoorPositionCalculated = true;
			}

			currentPosition.X += WallWidth;
		}
		else if (letter == ' ')
		{
			currentPosition.X += WallWidth;
		}

		stringIndex += 1;
		if (!rowCountCalculated) {
			rowCount += 1;
		}
	}

	columnCount += 1;
	rowCount -= 1;

	_rowCount = rowCount;
	_columnCount = columnCount;

	//GetWorld()->SpawnActor(TestDoorPrefab, &_topRowDoorPosition, &FRotator::ZeroRotator);
	//GetWorld()->SpawnActor(TestDoorPrefab, &_leftColumnDoorPosition, &FRotator::ZeroRotator);
	//GetWorld()->SpawnActor(TestDoorPrefab, &_rightColumnDoorPosition, &FRotator::ZeroRotator);
	//GetWorld()->SpawnActor(TestDoorPrefab, &_bottomRowDoorPosition, &FRotator::ZeroRotator);
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
