// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "RoomGenerator.h"

// Sets default values
ATile::ATile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TileParent = CreateDefaultSubobject<USceneComponent>(TEXT("TileParent"));
	RootComponent = TileParent;

	TileCenterOffset = FVector(100, 100, 0);
	_pickupType = PickupType::None;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	TileCenter = GetActorLocation() + TileCenterOffset;
	_isMoveable = false;
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATile::SetTileParent(ARoomGenerator* roomGenerator)
{
	_roomParent = roomGenerator;
}

void ATile::SetPositionInRoom(int row, int column)
{
	_row = row;
	_column = column;
}


bool ATile::IsTileMarked()
{
	return _isMoveable;
}

void ATile::MarkTileMoveable(UMaterialInstance* markedMaterial)
{
	_isMoveable = true;
	SetTileMarkedMaterial(markedMaterial);
}

void ATile::ClearTileMoveableStatus(UMaterialInstance* defaultMaterial)
{
	_isMoveable = false;
	SetTileUnMarkedMaterial(defaultMaterial);
}

int ATile::GetRow()
{
	return _row;
}

int ATile::GetColumn()
{
	return  _column;
}

void ATile::SetPickupType(PickupType pickupType)
{
	_pickupType = pickupType;
}

PickupType ATile::GetPickupType()
{
	return  _pickupType;
}
