// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class ARoomGenerator;

UCLASS()
class PROCGENWORLD_API ATile : public AActor
{
	GENERATED_BODY()

		ARoomGenerator* _roomParent;
	bool _isMoveable;

	int _row;
	int _column;

public:
#pragma region Properties

	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadWrite)
		USceneComponent* TileParent;

	UPROPERTY(BlueprintReadOnly)
		FVector TileCenter;

	UPROPERTY(Category = Positions, EditAnywhere)
		FVector TileCenterOffset;

#pragma endregion

	// Sets default values for this actor's properties
	ATile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category = Display, BlueprintImplementableEvent)
		void SetTileMarkedMaterial(UMaterialInstance* markedMaterial);

	UFUNCTION(Category = Display, BlueprintImplementableEvent)
		void SetTileUnMarkedMaterial(UMaterialInstance* defaultMaterial);

	void SetTileParent(ARoomGenerator* roomGenerator);
	void SetPositionInRoom(int row, int column);

	bool IsTileMarked();
	void MarkTileMoveable(UMaterialInstance* markedMaterial);
	void ClearTileMoveableStatus(UMaterialInstance* defaultMaterial);

	int GetRow();
	int GetColumn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
