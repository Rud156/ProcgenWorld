// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UCLASS()
class PROCGENWORLD_API ATile : public AActor
{
	GENERATED_BODY()

	bool _isMoveable;

public:
#pragma region Properties

	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadWrite)
		USceneComponent* TileParent;

	UPROPERTY(BlueprintReadOnly)
		FVector TileCenter;

#pragma endregion

	// Sets default values for this actor's properties
	ATile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category = Display, BlueprintImplementableEvent)
		void SetTileMarkedMaterial(UMaterialInstance* markedMaterial);

	UFUNCTION(Category = Display, BlueprintImplementableEvent)
		void SetTileUnMarkedMaterial(UMaterialInstance* defaultMaterial);

	bool IsTileMarked();
	void MarkTileMoveable(UMaterialInstance* markedMaterial);
	void ClearTileMoveableStatus(UMaterialInstance* defaultMaterial);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
