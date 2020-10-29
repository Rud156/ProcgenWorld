// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Data/EnumData.h"
#include "Tile.generated.h"

class ARoomGenerator;

UCLASS()
class PROCGENWORLD_API ATile : public AActor
{
	GENERATED_BODY()

	ARoomGenerator* _roomParent;
	bool _isInteractible;

	PickupType _pickupType;
	AActor* _pickupItem;

	int _row;
	int _column;

	TileType _tileType;

public:
#pragma region Properties

	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadWrite)
		USceneComponent* TileParent;

	UPROPERTY(BlueprintReadOnly)
		FVector TileCenter;

	UPROPERTY(Category = Positions, EditAnywhere)
		FVector TileCenterOffset;

	UPROPERTY(Category = Pickups, EditAnywhere)
		TMap<PickupType, TSubclassOf<class AActor>> Pickups;

	UPROPERTY(Category = Display, EditAnywhere)
		UMaterialInstance* FloorMaterial;

	UPROPERTY(Category = Display, EditAnywhere)
		UMaterialInstance* UpgradeMaterial;

	UPROPERTY(Category = Display, EditAnywhere)
		UMaterialInstance* VictoryMaterial;

	UPROPERTY(Category = Display, EditAnywhere)
		UMaterialInstance* LavaMaterial;

#pragma endregion

	ATile();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category = Display, BlueprintImplementableEvent)
		void SetTileMaterial(UMaterialInstance* material);

	UFUNCTION(Category = Display, BlueprintImplementableEvent)
		void SetOutlineStatus(bool isVisible);

	void SetTileParent(ARoomGenerator* roomGenerator);
	ARoomGenerator* GetTileParentRoom();
	void SetPositionInRoom(int row, int column);

	bool IsTileMarked();
	void MarkTileInteractible();
	void ClearTileMarkedStatus();

	int GetRow();
	int GetColumn();

	void SetTileType(TileType tileType);
	TileType GetTileType();

	void SetPickupType(PickupType pickupType);
	void ClearPickup();
	PickupType GetPickupType();

protected:
	virtual void BeginPlay() override;

};
