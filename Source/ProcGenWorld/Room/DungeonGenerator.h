// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/RandomStream.h"

#include "GameFramework/Actor.h"

#include "DungeonGenerator.generated.h"

UCLASS()
class PROCGENWORLD_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()

private:
	FRandomStream _stream;
	enum class Side {
		Left,
		Right,
		Top,
		Bottom
	};

	TArray<TArray<FString>> _roomMatrix;
	TArray<TArray<int>> _roomCounter;
	TMap<int, TArray<int>> _adjacencyList;

	int _minRow;
	int _maxRow;
	int _minColumn;
	int _maxColumn;

	int _spawnRoomRow;
	int _spawnRoomColumn;

	int _exitRoomRow;
	int _exitRoomColumn;

	int _randomSeed;

	void GenerateRoomBase();
	void UpdateMinMaxRowColumn(int i, int j);
	void UpdateRoomMatrixCounter();
	void UpdateRoomAdjacencyList();
	bool CheckBFSForExitRoom();

	void PlaceSingleDoorRooms();
	void MergeSingleDoorRooms();

	TArray<int> GetAdjacentRoomExits(int row, int column);
	FString GetRoomWithExits(TArray<FString> roomNames, int exitLeft, int exitRight, int exitTop, int exitBottom);
	TArray<FString> GetAllRoomWithExits(TArray<FString> rooms, int exitLeft, int exitRight, int exitTop, int exitBottom);

	TArray<FString> GetRoomsWithLeftExit(TArray<FString> rooms);
	TArray<FString> GetRoomsWithRightExit(TArray<FString> rooms);
	TArray<FString> GetRoomsWithTopExit(TArray<FString> rooms);
	TArray<FString> GetRoomsWithBottomExit(TArray<FString> rooms);

	TArray<FString> RemoveRoomsWithLeftExit(TArray<FString> rooms);
	TArray<FString> RemoveRoomsWithRightExit(TArray<FString> rooms);
	TArray<FString> RemoveRoomsWithTopExit(TArray<FString> rooms);
	TArray<FString> RemoveRoomsWithBottomExit(TArray<FString> rooms);

	bool RoomHasLeftExit(FString roomName);
	bool RoomHasRightExit(FString roomName);
	bool RoomHasTopExit(FString roomName);
	bool RoomHasBottomExit(FString roomName);

	TArray<FString> GetRoomWithNonSingleExit(TArray<FString> roomNames);
	Side SelectRandomExit(FString roomName);

public:
#pragma region Properties

	UPROPERTY(Category = Rooms, EditAnywhere)
		bool UseRandomSeed;

	UPROPERTY(Category = Rooms, EditAnywhere)
		int RandomSeed;

	UPROPERTY(Category = Rooms, ExitAnywhere)
		int SearchDepth;

	UPROPERTY(Category = Rooms, EditAnywhere)
		TArray<FString> RoomNames;

	UPROPERTY(Category = Rooms, EditAnywhere)
		TSubclassOf<class ARoomGenerator> RoomGenerator;

#pragma endregion

	// Sets default values for this actor's properties
	ADungeonGenerator();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
