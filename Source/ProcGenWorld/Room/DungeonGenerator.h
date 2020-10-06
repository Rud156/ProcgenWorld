// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/RandomStream.h"

#include "GameFramework/Actor.h"
#include "DungeonGenerator.generated.h"

class ARoomGenerator;

UCLASS()
class PROCGENWORLD_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()

private:
	enum class Side {
		Left,
		Right,
		Top,
		Bottom
	};

	static const int DEPTH_MARKER = -9999;
	FRandomStream _stream;

	TMap<int, TMap<int, FString>> _roomMatrix;
	TMap<int, TMap<int, int>> _roomMatrixCounter;
	TMap<int, TMap<int, ARoomGenerator*>> _rooms;
	TMap<int, TArray<int>> _roomAdjacencyList;

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
	void SpawnRooms();
	void AdjustRoomPositions();

	void UpdateMinMaxRowColumn(int i, int j);
	void UpdateRoomMatrixCounter();

	void UpdateRoomAdjacencyList();
	bool CheckBFSForExitRoom();
	int FindPathToSpawnRoom(int spawnRoomNumber, int startRoomNumber);
	TArray<int> GetRoomRowAndColumn(int roomNumber);

	void PlaceSingleDoorRooms();
	void MergeSingleDoorRooms();
	void AdjustRoomEdges();

	void PrintRooms();

	TArray<int> GetAdjacentRoomExits(int row, int column);

	FString GetRoomWithExits(TArray<FString> roomNames, int exitLeft, int exitRight, int exitTop, int exitBottom);
	FString GetRoomWithSpecificExits(TArray<FString> roomNames, int exitLeft, int exitRight, int exitTop, int exitBottom);
	TArray<FString> GetAllRoomsWithExits(TArray<FString> rooms, int exitLeft, int exitRight, int exitTop, int exitBottom);

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
	bool RoomHasSingleExit(FString roomName);

	TArray<FString> GetRoomWithNonSingleExit(TArray<FString> roomNames);
	Side SelectRandomExit(FString roomName);

public:
#pragma region Properties

	UPROPERTY(Category = Rooms, EditAnywhere)
		bool UseCustomSeed;

	UPROPERTY(Category = Rooms, EditAnywhere)
		int CustomSeed;

	UPROPERTY(Category = Rooms, EditAnywhere)
		int SearchDepth;

	UPROPERTY(Category = Rooms, EditAnywhere)
		FVector SpawnRoomPoint;

	UPROPERTY(Category = Rooms, EditAnywhere)
		TArray<FString> RoomNames;

	UPROPERTY(Category = Rooms, EditAnywhere)
		TSubclassOf<class ARoomGenerator> RoomGenerator;

#pragma endregion

	// Sets default values for this actor's properties
	ADungeonGenerator();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category = Rooms, BlueprintCallable)
		int GetSeed();

	UFUNCTION(Category = Rooms, BlueprintCallable)
		void RegenrateRooms();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
