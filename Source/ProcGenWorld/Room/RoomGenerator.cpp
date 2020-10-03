// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomGenerator.h"

// Sets default values
ARoomGenerator::ARoomGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_topRowDoorPosition = FVector::ZeroVector;
	_bottomRowDoorPosition = FVector::ZeroVector;
	_leftColumnDoorPosition = FVector::ZeroVector;
	_rightColumnDoorPosition = FVector::ZeroVector;
}

// Called when the game starts or when spawned
void ARoomGenerator::BeginPlay()
{
	Super::BeginPlay();

	FString fileName = "TestRoom.txt";

	FString directory = FPaths::ProjectContentDir() + "/Data";
	FString result;

	FString filePath = directory + "/" + fileName;

	IPlatformFile& fileManager = FPlatformFileManager::Get().GetPlatformFile();
	if (fileManager.FileExists(*filePath)) {
		FFileHelper::LoadFileToString(result, *filePath, FFileHelper::EHashOptions::None);

		RenderRoomFromString(result, FVector(0, 560, 0));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "File Not Found");
	}
}

// Called every frame
void ARoomGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
			GetWorld()->SpawnActor(WallPrefab, &currentPosition, &FRotator::ZeroRotator);

			currentPosition.X += WallWidth;
			lastSpawnVertical = false;
		}
		else if (letter == '|')
		{
			auto nextLetter = roomString[stringIndex + 1];

			if (nextLetter != ' ') {
				currentPosition.X += WallWidth - WallThickness * 1.25f;
			}

			GetWorld()->SpawnActor(WallPrefab, &currentPosition, &rotation90);

			currentPosition.X += WallWidth;
			lastSpawnVertical = true;
		}
		else if (letter == '\n')
		{
			auto nextLetter = roomString[stringIndex + 1];
			if (nextLetter == '-') {
				leftDoorPositionCalculated = true;
				rightDoorPositionCalculated = true;
			}

			if (!lastSpawnVertical) {
				currentPosition.Y += WallThickness;
			}
			else {
				currentPosition.Y += WallWidth;
			}

			currentPosition.X = startPosition.X + WallThickness * 1.25f;

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

FVector ARoomGenerator::GetStartPosition()
{
	return _startPosition;
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

FVector ARoomGenerator::GetRightColumnRowDoorPosition()
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
