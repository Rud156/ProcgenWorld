// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "../Data/EnumData.h"

#include "RoomGenerator.generated.h"

class ATile;
class AEnemyControllerBase;
class APlayerTopDownController;

UCLASS()
class PROCGENWORLD_API ARoomGenerator : public AActor
{
	GENERATED_BODY()

private:
	int _rowCount;
	int _columnCount;
	TMap<int, TMap<int, FWindowsPlatformTypes::TCHAR>> _room;
	TMap<int, TMap<int, ATile*>> _floorMatrix;

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

	UPROPERTY(Category = Enemy, EditAnywhere)
		TArray<TSubclassOf<class AEnemyControllerBase>> Enemies;

	UPROPERTY(Category = Display, EditAnywhere)
		UMaterialInstance* SpawnMaterial;

	UPROPERTY(Category = Display, EditAnywhere)
		UMaterialInstance* ExitMaterial;

	UPROPERTY(Category = Display, EditAnywhere)
		UMaterialInstance* GeneralMaterial;

	UPROPERTY(Category = Display, EditAnywhere)
		UMaterialInstance* TileMarkerMaterial;

	UPROPERTY(Category = WallMovement, EditAnywhere)
		float LerpSpeed;

#pragma endregion

	ARoomGenerator();
	virtual void Tick(float DeltaTime) override;

	void LoadRoomFromFile(FString roomName, FVector startPosition);

	void HandleUnitDied(AEnemyControllerBase* enemy);

	ATile* GetRandomTileInRoom(int& row, int& column);
	ATile* GetTileAtPosition(int row, int column);
	AEnemyControllerBase* GetEnemyAtPosition(int row, int column);
	void ClearAllTilesStatus();
	void MarkAdjacentMovementSpots(int currentRow, int currentColumn);
	void MarkTile(int row, int column);

	TMap<int, TMap<int, WorldElementType>> GetWorldState();

	FString GetRoomName();
	FVector GetStartPosition();
	void UpdateRoomPosition(FVector offset);

	FVector GetTopRowDoorPosition();
	FVector GetBottomRowDoorPosition();
	FVector GetLeftColumnDoorPosition();
	FVector GetRightColumnDoorPosition();

	int GetRowCount();
	int GetColumnCount();

	void SetPlayerController(APlayerTopDownController* playerController);
	void SpawnEnemies();
	void ClearAllEnemies();
	TArray<AEnemyControllerBase*> GetEnemies();

	UFUNCTION(Category = Display, BlueprintCallable, BlueprintPure)
		bool IsPlayerInRoom();

	UFUNCTION(Category = Display, BlueprintCallable, BlueprintPure)
		bool IsRoomCleared();
	void SetRoomCleared();

	void SpawnRoomDoors();
	void DestroyRoomDoors();

	UFUNCTION(Category = Display, BlueprintCallable, BlueprintPure)
		int GetRoomDepth();
	void SetRoomDepth(int roomDepth);

	UFUNCTION(Category = Room, BlueprintCallable, BlueprintPure)
		TArray<ATile*> GetFloorTiles();

	UFUNCTION(Category = Room, BlueprintCallable, BlueprintPure)
		TArray<AActor*> GetWalls();

	UFUNCTION(Category = Display, BlueprintCallable)
		void SetFloorColor(int roomType, UStaticMeshComponent* mesh);

	UFUNCTION(Category = Display, BlueprintImplementableEvent)
		void UpdateFloorMaterial(int roomType);

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
};
