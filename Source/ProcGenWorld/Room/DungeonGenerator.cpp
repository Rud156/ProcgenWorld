// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonGenerator.h"

// Sets default values
ADungeonGenerator::ADungeonGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_minRow = 999;
	_maxRow = -1;
	_minColumn = 999;
	_maxColumn = -1;

	_spawnRoomRow = -1;
	_spawnRoomColumn = -1;

	_exitRoomRow = -1;
	_exitRoomColumn = -1;
}

// Called when the game starts or when spawned
void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();

	if (!UseCustomSeed) {
		_randomSeed = FMath::Rand();
		_stream = FRandomStream(_randomSeed);
	}
	else {
		_randomSeed = CustomSeed;
		_stream = FRandomStream(CustomSeed);
	}

	GenerateRoomBase();
}

// Called every frame
void ADungeonGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADungeonGenerator::GenerateRoomBase()
{
	_spawnRoomRow = 100;
	_spawnRoomColumn = 100;

	int i = _spawnRoomRow;
	int j = _spawnRoomColumn;

	auto validSpawnRooms = GetRoomWithNonSingleExit(RoomNames);
	int randomSpawnRoom = _stream.RandRange(0, validSpawnRooms.Num() - 1);

	_roomMatrix[i] = TMap<int, FString>();
	_roomMatrix[i][j] = validSpawnRooms[randomSpawnRoom];

	while (true) {
		auto randomSide = SelectRandomExit(_roomMatrix[i][j]);

		if (randomSide == Side::Left) {
			j -= 1;
		}
		else if (randomSide == Side::Right) {
			j += 1;
		}
		else if (randomSide == Side::Top) {
			i -= 1;
		}
		else {
			i += 1;
		}

		if (i == _spawnRoomRow && j == _spawnRoomColumn) {
			continue;
		}

		UpdateMinMaxRowColumn(i, j);

		if (!_roomMatrix.Contains(i)) {
			_roomMatrix[i] = TMap<int, FString>();
		}

		auto exits = GetAdjacentRoomExits(i, j);

		int leftSide = j - 1;
		int rightSide = j + 1;
		int topSide = i - 1;
		int bottomSide = i + 1;

		FString spawnRoom = _roomMatrix[_spawnRoomRow][_spawnRoomColumn];
		FString room;

		if (leftSide == _spawnRoomColumn && i == _spawnRoomRow && RoomHasRightExit(spawnRoom)) {
			room = GetRoomWithExits(RoomNames, 1, exits[1], exits[2], exits[3]);
		}
		else if (rightSide == _spawnRoomColumn && i == _spawnRoomRow && RoomHasLeftExit(spawnRoom)) {
			room = GetRoomWithExits(RoomNames, exits[0], 1, exits[2], exits[3]);
		}
		else if (j == _spawnRoomColumn && topSide == _spawnRoomRow && RoomHasBottomExit(spawnRoom)) {
			room = GetRoomWithExits(RoomNames, exits[0], exits[1], 1, exits[3]);
		}
		else if (j == _spawnRoomColumn && bottomSide == _spawnRoomRow && RoomHasTopExit(spawnRoom)) {
			room = GetRoomWithExits(RoomNames, exits[0], exits[1], exits[2], 1);
		}
		else {
			room = GetRoomWithExits(RoomNames, exits[0], exits[1], exits[2], exits[3]);
		}

		_roomMatrix[i][j] = room;

		UpdateRoomMatrixCounter();
		UpdateRoomAdjacencyList();

		FString text = "Left: " + FString::SanitizeFloat(exits[0]) + ", Right: " + FString::SanitizeFloat(exits[1]) + ", Top: " \
			+ FString::SanitizeFloat(exits[2]) + ", Bottom: " + FString::SanitizeFloat(exits[3]);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *text);

		UE_LOG(LogTemp, Warning, TEXT("%s"), *_roomMatrix[i][j]);
		PrintRooms();

		bool breakLoop = CheckBFSForExitRoom();
		if (breakLoop) {
			break;
		}
	}

	PlaceSingleDoorRooms();
	MergeSingleDoorRooms();
	AdjustRoomEdges();
}

void ADungeonGenerator::UpdateMinMaxRowColumn(int i, int j)
{
	if (_minRow > i) {
		_minRow = i;
	}
	if (_maxRow < i) {
		_maxRow = i;
	}

	if (_minColumn > j) {
		_minColumn = j;
	}
	if (_maxColumn < j) {
		_maxColumn = j;
	}
}

void ADungeonGenerator::UpdateRoomMatrixCounter()
{
	int staticCounter = 0;

	for (int i = 0; i <= _maxRow; i++) {
		for (int j = _minColumn; j < _maxColumn; j++) {
			if (_roomMatrix[i].Contains(j)) {

				if (!_roomMatrixCounter.Contains(i)) {
					_roomMatrixCounter[i] = TMap<int, int>();
				}

				_roomMatrixCounter[i][j] = staticCounter;
				staticCounter += 1;
			}
		}
	}
}

void ADungeonGenerator::UpdateRoomAdjacencyList()
{
	_roomAdjacencyList = TMap<int, TArray<int>>();

	for (int i = _minRow; i <= _maxRow; i++) {
		for (int j = _minColumn; j <= _maxColumn; j++) {
			if (_roomMatrix[i].Contains(j)) {

				TArray<int> adjacentRooms;
				int roomNumber = _roomMatrixCounter[i][j];

				bool hasLeftExit = false;
				bool hasRightExit = false;
				bool hasTopExit = false;
				bool hasBottomExit = false;

				if (RoomHasLeftExit(_roomMatrix[i][j])) {
					hasLeftExit = true;
				}

				if (RoomHasRightExit(_roomMatrix[i][j])) {
					hasRightExit = true;
				}

				if (RoomHasTopExit(_roomMatrix[i][j])) {
					hasTopExit = true;
				}

				if (RoomHasBottomExit(_roomMatrix[i][j])) {
					hasBottomExit = true;
				}

				int leftSide = j - 1;
				int rightSide = j + 1;
				int topSide = i - 1;
				int bottomSide = i + 1;

				if (hasLeftExit) {
					if (_roomMatrix[i].Contains(leftSide) && RoomHasRightExit(_roomMatrix[i][leftSide])) {
						adjacentRooms.Add(_roomMatrixCounter[i][leftSide]);
					}
				}

				if (hasRightExit) {
					if (_roomMatrix[i].Contains(rightSide) && RoomHasLeftExit(_roomMatrix[i][rightSide])) {
						adjacentRooms.Add(_roomMatrixCounter[i][rightSide]);
					}
				}

				if (hasTopExit && _roomMatrix.Contains(topSide)) {
					if (_roomMatrix[topSide].Contains(j) && RoomHasBottomExit(_roomMatrix[topSide][j])) {
						adjacentRooms.Add(_roomMatrixCounter[topSide][j]);
					}
				}

				if (hasBottomExit && _roomMatrix.Contains(bottomSide)) {
					if (_roomMatrix[bottomSide].Contains(j) && RoomHasTopExit(_roomMatrix[bottomSide][j])) {
						adjacentRooms.Add(_roomMatrixCounter[bottomSide][j]);
					}
				}

				_roomAdjacencyList[roomNumber] = adjacentRooms;
			}
		}
	}
}

bool ADungeonGenerator::CheckBFSForExitRoom()
{
	for (int i = _minRow; i <= _maxRow; i++) {
		for (int j = _minColumn; j <= _maxColumn; j++) {
			if (i == _spawnRoomRow && j == _spawnRoomColumn) {
				// Do nothing here...
			}
			else if (_roomMatrix[i].Contains(j)) {
				int roomNumber = _roomMatrixCounter[i][j];
				int spawnRoomNumber = _roomMatrixCounter[_spawnRoomRow][_spawnRoomColumn];
				int pathLength = FindPathToSpawnRoom(spawnRoomNumber, roomNumber);

				if (pathLength == SearchDepth) {
					_exitRoomRow = i;
					_exitRoomColumn = j;

					return true;
				}
			}
		}
	}

	return false;
}

int ADungeonGenerator::FindPathToSpawnRoom(int spawnRoomNumber, int startRoomNumber)
{
	TSet<int> explored;
	TArray<int> queue = { startRoomNumber, ARRAY_NIL };
	int pathLength = 0;

	if (spawnRoomNumber == startRoomNumber) {
		return 0;
	}

	while (queue.Num() > 1) {
		int node = queue[0];
		queue.RemoveAt(0);

		if (node == ARRAY_NIL) {
			pathLength = pathLength + 1;
			queue.Add(ARRAY_NIL);
		}
		else if (!explored.Contains(node)) {
			auto adjacentRooms = _roomAdjacencyList[node];

			for (int i = 0; i < adjacentRooms.Num(); i++) {
				auto room = adjacentRooms[i];
				queue.Add(room);

				if (room == spawnRoomNumber) {
					return pathLength;
				}
			}

			explored.Add(node);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Can't reach Spawn Room"));
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Can't reach Spawn Room");

	return -1;
}

void ADungeonGenerator::PlaceSingleDoorRooms()
{
	UE_LOG(LogTemp, Warning, TEXT("Placing Single Door Rooms"));

	int minRow = _minRow;
	int maxRow = _maxRow;
	int minCoulmn = _minColumn;
	int maxColumn = _maxColumn;

	for (int i = _minRow; i <= _maxRow; i++) {
		for (int j = _minColumn; j <= _maxColumn; j++) {
			FString tempString = "I: " + FString::SanitizeFloat(i) + ", J: " + FString::SanitizeFloat(j);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *tempString);

			if (_roomMatrix[i].Contains(j)) {
				UE_LOG(LogTemp, Warning, TEXT("%s"), *_roomMatrix[i][j]);

				int leftSide = j - 1;
				int rightSide = j + 1;
				int topSide = i - 1;
				int bottomSide = i + 1;

				if (!_roomMatrix[i].Contains(leftSide) && RoomHasLeftExit(_roomMatrix[i][j])) {
					UE_LOG(LogTemp, Warning, TEXT("Left Added"));

					FString room = GetRoomWithExits(RoomNames, -1, 1, -1, -1);
					_roomMatrix[i][leftSide] = room;

					if (minCoulmn > leftSide) {
						minCoulmn = leftSide;
					}
				}

				if (!_roomMatrix[i].Contains(rightSide) && RoomHasRightExit(_roomMatrix[i][j])) {
					UE_LOG(LogTemp, Warning, TEXT("Right Added"));

					FString room = GetRoomWithExits(RoomNames, 1, -1, -1, -1);
					_roomMatrix[i][rightSide] = room;

					if (_maxColumn < rightSide) {
						_maxColumn = rightSide;
					}
				}

				if (RoomHasTopExit(_roomMatrix[i][j])) {
					if (!_roomMatrix.Contains(topSide)) {
						UE_LOG(LogTemp, Warning, TEXT("Top Added 0"));

						FString room = GetRoomWithExits(RoomNames, -1, -1, -1, 1);

						_roomMatrix[topSide] = TMap<int, FString>();
						_roomMatrix[topSide][j] = room;
					}
					else if (!_roomMatrix[topSide].Contains(j)) {
						UE_LOG(LogTemp, Warning, TEXT("Top Added 1"));

						FString room = GetRoomWithExits(RoomNames, -1, -1, -1, 1);
						_roomMatrix[topSide][j] = room;
					}

					if (_minRow > topSide) {
						_minRow = topSide;
					}
				}

				if (RoomHasBottomExit(_roomMatrix[i][j])) {
					if (!_roomMatrix.Contains(bottomSide)) {
						UE_LOG(LogTemp, Warning, TEXT("Bottom Added 0"));

						FString room = GetRoomWithExits(RoomNames, -1, -1, 1, -1);

						_roomMatrix[bottomSide] = TMap<int, FString>();
						_roomMatrix[bottomSide][j] = room;
					}
					else if (!_roomMatrix[bottomSide].Contains(j)) {
						UE_LOG(LogTemp, Warning, TEXT("Bottom Added 1"));

						FString room = GetRoomWithExits(RoomNames, -1, -1, 1, -1);
						_roomMatrix[bottomSide][j] = room;
					}

					if (_maxRow < bottomSide) {
						_maxRow = bottomSide;
					}
				}
			}
		}
	}

	_minRow = minRow;
	_maxRow = maxRow;
	_minColumn = minCoulmn;
	_maxColumn = maxColumn;

	UE_LOG(LogTemp, Warning, TEXT("Min Row: %d"), _minRow);
	UE_LOG(LogTemp, Warning, TEXT("Max Row: %d"), _maxRow);
	UE_LOG(LogTemp, Warning, TEXT("Min Column: %d"), _minColumn);
	UE_LOG(LogTemp, Warning, TEXT("Max Column: %d"), _maxColumn);
	PrintRooms();
}

void ADungeonGenerator::MergeSingleDoorRooms()
{
	UE_LOG(LogTemp, Warning, TEXT("Merging Rooms"));

	for (int i = _minRow; i <= _maxRow; i++) {
		for (int j = _minColumn; j <= _maxColumn; j++) {
			if (_roomMatrix[i].Contains(j) && RoomHasSingleExit(_roomMatrix[i][j])) {
				auto exits = GetAdjacentRoomExits(i, j);
				int totalExits = 0;

				if (exits[0] == 1) {
					totalExits += 1;
				}
				if (exits[1] == 1) {
					totalExits += 1;
				}
				if (exits[2] == 1) {
					totalExits += 1;
				}
				if (exits[3] == 1) {
					totalExits += 1;
				}

				UE_LOG(LogTemp, Warning, TEXT("Total Exits: %d, Room: %s"), totalExits, *_roomMatrix[i][j]);

				if (totalExits > 1) {
					FString room = GetRoomWithSpecificExits(RoomNames, exits[0], exits[1], exits[2], exits[3]);
					_roomMatrix[i][j] = room;
				}
			}
		}
	}
}

void ADungeonGenerator::AdjustRoomEdges()
{
	bool allEmpty = true;

	// Top Row
	for (int i = _minColumn; i <= _maxColumn; i++) {
		if (_roomMatrix[_minRow].Contains(i)) {
			allEmpty = false;
			break;
		}
	}
	if (allEmpty) {
		_roomMatrix.Remove(_minRow);
		_minRow += 1;
	}

	// Bottom Row
	allEmpty = true;
	for (int i = _minColumn; i <= _maxColumn; i++) {
		if (_roomMatrix[_maxRow].Contains(i)) {
			allEmpty = false;
			break;
		}
	}
	if (allEmpty) {
		_roomMatrix.Remove(_maxRow);
		_maxRow -= 1;
	}

	// Left Column
	allEmpty = true;
	for (int i = _minRow; i <= _maxRow; i++) {
		if (_roomMatrix[i].Contains(_minColumn)) {
			allEmpty = false;
			break;
		}
	}
	if (allEmpty) {
		_minColumn += 1;
	}

	// Right Column
	allEmpty = true;
	for (int i = _minRow; i <= _maxRow; i++) {
		if (_roomMatrix[i].Contains(_maxColumn)) {
			allEmpty = false;
			break;
		}
	}
	if (allEmpty) {
		_maxColumn -= 1;
	}
}

void ADungeonGenerator::PrintRooms()
{
	UE_LOG(LogTemp, Warning, TEXT("Printing Rooms\n"));

	for (int i = _minRow; i <= _maxRow; i++) {
		FString stringValue = "";

		for (int j = _minColumn; j <= _maxColumn; j++) {
			if (_roomMatrix.Contains(i)) {
				if (_roomMatrix[i].Contains(j)) {
					stringValue = stringValue + _roomMatrix[i][j] + " ";
				}
				else {
					stringValue = stringValue + "*** ";
				}
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("%s"), *stringValue);
	}
}

TArray<int> ADungeonGenerator::GetAdjacentRoomExits(int row, int column)
{
	int leftSide = column - 1;
	int rightSide = column + 1;
	int topSide = row - 1;
	int botttomSide = row + 1;

	int exitLeft = 0;
	int exitRight = 0;
	int exitTop = 0;
	int exitBottom = 0;

	if (_roomMatrix[row].Contains(leftSide)) {
		if (RoomHasRightExit(_roomMatrix[row][leftSide])) {
			exitLeft = 1;
		}
		else {
			exitLeft = -1;
		}
	}

	if (_roomMatrix[row].Contains(rightSide)) {
		if (RoomHasLeftExit(_roomMatrix[row][leftSide])) {
			exitRight = 1;
		}
		else {
			exitRight = -1;
		}
	}

	if (_roomMatrix.Contains(topSide)) {
		if (_roomMatrix[topSide].Contains(column)) {
			if (RoomHasBottomExit(_roomMatrix[topSide][column])) {
				exitTop = 1;
			}
			else {
				exitTop = -1;
			}
		}
	}

	if (_roomMatrix.Contains(botttomSide)) {
		if (_roomMatrix[botttomSide].Contains(column)) {
			if (RoomHasTopExit(_roomMatrix[botttomSide][column])) {
				exitBottom = 1;
			}
			else {
				exitBottom = -1;
			}
		}
	}

	TArray<int> roomExits = { exitLeft, exitRight, exitTop, exitBottom };
	return roomExits;
}

FString ADungeonGenerator::GetRoomWithExits(TArray<FString> roomNames, int exitLeft, int exitRight, int exitTop, int exitBottom)
{
	auto validRooms = GetAllRoomsWithExits(roomNames, exitLeft, exitRight, exitTop, exitBottom);

	if (exitLeft == -1) {
		validRooms = RemoveRoomsWithLeftExit(validRooms);
	}

	if (exitRight == -1) {
		validRooms = RemoveRoomsWithRightExit(validRooms);
	}

	if (exitTop == -1) {
		validRooms = RemoveRoomsWithTopExit(validRooms);
	}

	if (exitBottom == -1) {
		validRooms = RemoveRoomsWithBottomExit(validRooms);
	}

	int randomIndex = _stream.RandRange(0, validRooms.Num() - 1);
	return validRooms[randomIndex];
}

FString ADungeonGenerator::GetRoomWithSpecificExits(TArray<FString> roomNames, int exitLeft, int exitRight, int exitTop, int exitBottom)
{
	TArray<FString> validRooms = GetAllRoomsWithExits(roomNames, exitLeft, exitRight, exitTop, exitBottom);
	TArray<FString> actualValidRooms;

	for (int i = 0; i < validRooms.Num(); i++) {
		bool roomMarkedForRemoval = false;

		if (exitLeft != 1 && RoomHasLeftExit(validRooms[i])) {
			roomMarkedForRemoval = true;
		}

		if (exitRight != 1 && RoomHasRightExit(validRooms[i])) {
			roomMarkedForRemoval = true;
		}

		if (exitTop != 1 && RoomHasTopExit(validRooms[i])) {
			roomMarkedForRemoval = true;
		}

		if (exitBottom != 1 && RoomHasBottomExit(validRooms[i])) {
			roomMarkedForRemoval = true;
		}

		if (!roomMarkedForRemoval) {
			actualValidRooms.Add(validRooms[i]);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("New Length: %d"), actualValidRooms.Num());
	if (actualValidRooms.Num() <= 0) {
		return "";
	}

	int randomIndex = _stream.RandRange(0, actualValidRooms.Num() - 1);
	return actualValidRooms[randomIndex];
}

TArray<FString> ADungeonGenerator::GetAllRoomsWithExits(TArray<FString> rooms, int exitLeft, int exitRight, int exitTop, int exitBottom)
{
	TArray<FString> validRooms = rooms;

	if (exitLeft == 1) {
		validRooms = GetRoomsWithLeftExit(validRooms);
	}

	if (exitRight == 1) {
		validRooms = GetRoomsWithRightExit(validRooms);
	}

	if (exitTop == 1) {
		validRooms = GetRoomsWithTopExit(validRooms);
	}

	if (exitBottom == 1) {
		validRooms = GetRoomsWithBottomExit(validRooms);
	}

	return validRooms;
}

TArray<FString> ADungeonGenerator::GetRoomsWithLeftExit(TArray<FString> rooms)
{
	TArray<FString> validRooms;

	for (int i = 0; i < rooms.Num(); i++) {
		if (RoomHasLeftExit(rooms[i])) {
			validRooms.Add(rooms[i]);
		}
	}

	return validRooms;
}

TArray<FString> ADungeonGenerator::GetRoomsWithRightExit(TArray<FString> rooms)
{
	TArray<FString> validRooms;

	for (int i = 0; i < rooms.Num(); i++) {
		if (RoomHasRightExit(rooms[i])) {
			validRooms.Add(rooms[i]);
		}
	}

	return validRooms;
}

TArray<FString> ADungeonGenerator::GetRoomsWithTopExit(TArray<FString> rooms)
{
	TArray<FString> validRooms;

	for (int i = 0; i < rooms.Num(); i++) {
		if (RoomHasTopExit(rooms[i])) {
			validRooms.Add(rooms[i]);
		}
	}

	return validRooms;
}

TArray<FString> ADungeonGenerator::GetRoomsWithBottomExit(TArray<FString> rooms)
{
	TArray<FString> validRooms;

	for (int i = 0; i < rooms.Num(); i++) {
		if (RoomHasBottomExit(rooms[i])) {
			validRooms.Add(rooms[i]);
		}
	}

	return validRooms;
}

TArray<FString> ADungeonGenerator::RemoveRoomsWithLeftExit(TArray<FString> rooms)
{
	TArray<FString> validRooms;

	for (int i = 0; i < rooms.Num(); i++) {
		if (!RoomHasLeftExit(rooms[i])) {
			validRooms.Add(rooms[i]);
		}
	}

	return validRooms;
}

TArray<FString> ADungeonGenerator::RemoveRoomsWithRightExit(TArray<FString> rooms)
{
	TArray<FString> validRooms;

	for (int i = 0; i < rooms.Num(); i++) {
		if (!RoomHasRightExit(rooms[i])) {
			validRooms.Add(rooms[i]);
		}
	}

	return validRooms;
}

TArray<FString> ADungeonGenerator::RemoveRoomsWithTopExit(TArray<FString> rooms)
{
	TArray<FString> validRooms;

	for (int i = 0; i < rooms.Num(); i++) {
		if (!RoomHasTopExit(rooms[i])) {
			validRooms.Add(rooms[i]);
		}
	}

	return validRooms;
}

TArray<FString> ADungeonGenerator::RemoveRoomsWithBottomExit(TArray<FString> rooms)
{
	TArray<FString> validRooms;

	for (int i = 0; i < rooms.Num(); i++) {
		if (!RoomHasBottomExit(rooms[i])) {
			validRooms.Add(rooms[i]);
		}
	}

	return validRooms;
}

bool ADungeonGenerator::RoomHasLeftExit(FString roomName)
{
	return roomName.Contains("L");
}

bool ADungeonGenerator::RoomHasRightExit(FString roomName)
{
	return roomName.Contains("R");
}

bool ADungeonGenerator::RoomHasTopExit(FString roomName)
{
	return roomName.Contains("T");
}

bool ADungeonGenerator::RoomHasBottomExit(FString roomName)
{
	return roomName.Contains("B");
}

bool ADungeonGenerator::RoomHasSingleExit(FString roomName)
{
	return false;
}

TArray<FString> ADungeonGenerator::GetRoomWithNonSingleExit(TArray<FString> roomNames)
{
	TArray<FString> validRooms = TArray<FString>();

	for (int i = 0; i < roomNames.Num(); i++) {
		FString roomName = RoomNames[i];

		bool hasLeft = RoomHasLeftExit(roomName);
		bool hasRight = RoomHasRightExit(roomName);
		bool hasTop = RoomHasTopExit(roomName);
		bool hasBottom = RoomHasBottomExit(roomName);

		TArray<int> tempArray = { 0, 0, 0, 0 };
		if (hasLeft) tempArray[0] = 1;
		if (hasRight) tempArray[1] = 1;
		if (hasTop) tempArray[2] = 1;
		if (hasBottom) tempArray[3] = 1;

		int totalSides = 0;
		for (int j = 0; j < tempArray.Num(); j++) {
			if (tempArray[j] == 1) {
				totalSides += 1;
			}
		}

		if (totalSides != 1) {
			validRooms.Add(roomName);
		}
	}

	return validRooms;
}

ADungeonGenerator::Side ADungeonGenerator::SelectRandomExit(FString roomName)
{
	TArray<int> randomArray = { 0, 0, 0, 0 };

	if (RoomHasLeftExit(roomName)) {
		randomArray[0] = 1;
	}
	if (RoomHasRightExit(roomName)) {
		randomArray[1] = 1;
	}
	if (RoomHasTopExit(roomName)) {
		randomArray[2] = 1;
	}
	if (RoomHasBottomExit(roomName)) {
		randomArray[3] = 1;
	}

	TArray<int> validIndex;
	for (int i = 0; i < randomArray.Num(); i++) {
		validIndex.Add(i);
	}

	int randomIndex = _stream.RandRange(0, validIndex.Num() - 1);
	int randomSide = validIndex[randomIndex];

	if (randomSide == 1) {
		return Side::Left;
	}
	else if (randomSide == 2) {
		return Side::Right;
	}
	else if (randomSide == 3) {
		return Side::Top;
	}
	else {
		return Side::Bottom;
	}
}

