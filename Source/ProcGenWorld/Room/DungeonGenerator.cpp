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

	if (!UseRandomSeed) {
		_randomSeed = FMath::Rand();
		_stream = FRandomStream(_randomSeed);
	}
	else {
		_randomSeed = RandomSeed;
		_stream = FRandomStream(RandomSeed);
	}
}

// Called every frame
void ADungeonGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADungeonGenerator::GenerateRoomBase()
{
	// TODO: Actually Assign Data to Array Or Unreal Will Crash

	_spawnRoomRow = 100;
	_spawnRoomColumn = 100;

	int i = _spawnRoomRow;
	int j = _spawnRoomColumn;

	auto validSpawnRooms = GetRoomWithNonSingleExit(RoomNames);
	int randomSpawnRoom = _stream.RandRange(0, validSpawnRooms.Num() - 1);

	_roomMatrix[i] = TArray<FString>();
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

		if (_roomMatrix[i] == NULL) {
			_roomMatrix[i] = {};
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

		bool breakLoop = CheckBFSForExitRoom();
		if (breakLoop) {
			break;
		}
	}

	PlaceSingleDoorRooms();
	MergeSingleDoorRooms();
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
}

void ADungeonGenerator::UpdateRoomAdjacencyList()
{
}

bool ADungeonGenerator::CheckBFSForExitRoom()
{
	return false;
}

void ADungeonGenerator::PlaceSingleDoorRooms()
{
}

void ADungeonGenerator::MergeSingleDoorRooms()
{
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

	if (_roomMatrix[row][leftSide] != "") {
		if (RoomHasRightExit(_roomMatrix[row][leftSide])) {
			exitLeft = 1;
		}
		else {
			exitLeft = -1;
		}
	}

	if (_roomMatrix[row][rightSide] != "") {
		if (RoomHasLeftExit(_roomMatrix[row][leftSide])) {
			exitRight = 1;
		}
		else {
			exitRight = -1;
		}
	}

	if (_roomMatrix[topSide] != NULL) {
		if (_roomMatrix[topSide][column] != "") {
			if (RoomHasBottomExit(_roomMatrix[topSide][column])) {
				exitTop = 1;
			}
			else {
				exitTop = -1;
			}
		}
	}

	if (_roomMatrix[botttomSide] != NULL) {
		if (_roomMatrix[botttomSide][column] != "") {
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
	auto validRooms = GetAllRoomWithExits(roomNames, exitLeft, exitRight, exitTop, exitBottom);

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

TArray<FString> ADungeonGenerator::GetAllRoomWithExits(TArray<FString> rooms, int exitLeft, int exitRight, int exitTop, int exitBottom)
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

