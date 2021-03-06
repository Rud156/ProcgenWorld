// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "../Data/EnumData.h"

#include "RoomGenerator.generated.h"

class ATile;
class AEnemyControllerBase;
class APlayerTopDownController;
class ADungeonGenerator;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRoomCleared);

UCLASS()
class PROCGENWORLD_API ARoomGenerator : public AActor
{
	GENERATED_BODY()

private:
	int _roomRow;
	int _roomColumn;
	
	int _rowCount;
	int _columnCount;
	TMap<int, TMap<int, FWindowsPlatformTypes::TCHAR>> _room;
	TMap<int, TMap<int, ATile*>> _floorMatrix;

	ADungeonGenerator* _dungeonGenerator;

	UMaterialInstance* _roomDefaultMaterial;
	FVector _startPosition;

	FVector _topRowDoorPosition;
	FVector _bottomRowDoorPosition;
	FVector _leftColumnDoorPosition;
	FVector _rightColumnDoorPosition;

	FString _roomName;
	TArray<AActor*> _walls;
	TArray<ATile*> _floorTiles;
	TArray<AActor*> _doors;

	int _roomDepth;

	FVector _lerpStartPosition;
	FVector _lerpTargetPosition;
	float _lerpAmount;
	bool _isLerpActive;

	TArray<AEnemyControllerBase*> _roomEnemies;
	APlayerTopDownController* _playerController;
	bool _playerEnteredRoom;
	bool _isRoomCleared;

	void RenderRoomFromString(FString roomString, FVector startPosition);
	void GenerateRoomMatrix(FString roomString);
	void RenderRoomEdges(FVector startPosition);

public:
#pragma region Parameters

	UPROPERTY(Category = Wall, EditAnywhere)
		TSubclassOf<class AActor> WallPrefab;

	UPROPERTY(Category = Wall, EditAnywhere)
		TSubclassOf<class AActor> FloorPrefab;

	UPROPERTY(Category = Wall, EditAnywhere)
		TSubclassOf<class AActor> DoorPrefab;

	UPROPERTY(Category = Wall, EditAnywhere)
		float WallWidth;

	UPROPERTY(Category = Wall, EditAnywhere)
		float WallThickness;

	UPROPERTY(Category = SpawnCounts, EditAnywhere)
		int MinLavaTiles;

	UPROPERTY(Category = SpawnCounts, EditAnywhere)
		int MaxLavaTiles;

	UPROPERTY(Category = SpawnCounts, EditAnywhere)
		int MinUpgradeTiles;
	
	UPROPERTY(Category = SpawnCounts, EditAnywhere)
		int MaxUpgradeTiles;

	UPROPERTY(Category = SpawnCounts, EditAnywhere)
		int MinPickupTiles;

	UPROPERTY(Category = SpawnCounts, EditAnywhere)
		int MaxPickupTiles;

	UPROPERTY(Category = Enemy, EditAnywhere)
		TArray<TSubclassOf<class AEnemyControllerBase>> Enemies;

	UPROPERTY(Category = WallMovement, EditAnywhere)
		float LerpSpeed;

	UPROPERTY(Category = Delegates, BlueprintAssignable)
		FRoomCleared OnRoomCleared;

#pragma endregion

	ARoomGenerator();
	virtual void Tick(float DeltaTime) override;

	void LoadRoomFromFile(FString roomName, FVector startPosition);

	ATile* GetRandomTileInRoom(int& row, int& column);
	ATile* GetTileAtPosition(int row, int column);
	AEnemyControllerBase* GetEnemyAtPosition(int row, int column);
	void MarkAdjacentMovementSpots(int currentRow, int currentColumn);

	void ClearAllTileMarkedStatus();
	void MarkTile(int row, int column);
	void UnMarkTile(int row, int column);

	TMap<int, TMap<int, WorldElementType>> GetWorldState();

	FString GetRoomName();
	void SetRoomRowAndColumn(int row, int column);
	int GetRoomRow();
	int GetRoomColumn();
	FVector GetStartPosition();
	void UpdateRoomPosition(FVector offset);

	FVector GetTopRowDoorPosition();
	FVector GetBottomRowDoorPosition();
	FVector GetLeftColumnDoorPosition();
	FVector GetRightColumnDoorPosition();

	int GetRowCount();
	int GetColumnCount();
	bool IsPositionInRoom(int row, int column);

	void CheckAndActivateRoom();

	void SetPlayerController(APlayerTopDownController* playerController);
	void SetDungeonGenerator(ADungeonGenerator* dungeonGenerator);
	
	void SpawnEnemies();
	void ClearAllEnemies();
	void HandleEnemyDied(AEnemyControllerBase* enemy);
	TArray<AEnemyControllerBase*> GetEnemies();

	UFUNCTION(Category = Display, BlueprintCallable, BlueprintPure)
		bool IsPlayerInRoom();

	UFUNCTION(Category = Display, BlueprintCallable, BlueprintPure)
		bool IsRoomCleared();
	void SetRoomCleared();

	void SpawnRoomDoors();
	void DestroyRoomDoors();

	void SpawnRoomTiles();
	void ClearRoomTiles();
	void ClearStatusTiles();

	UFUNCTION(Category = Display, BlueprintCallable, BlueprintPure)
		int GetRoomDepth();
	void SetRoomDepth(int roomDepth);

	UFUNCTION(Category = Room, BlueprintCallable, BlueprintPure)
		TArray<ATile*> GetFloorTiles();

	UFUNCTION(Category = Room, BlueprintCallable, BlueprintPure)
		TArray<AActor*> GetWalls();

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
};
