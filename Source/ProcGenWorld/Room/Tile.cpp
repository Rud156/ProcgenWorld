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
	_tileType = TileType::FloorTile;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	_isInteractible = false;
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TileCenter = GetActorLocation() + TileCenterOffset; // This needs to be the case since the ActorLocation is initially incorrect
}

void ATile::SetTileParent(ARoomGenerator* roomGenerator)
{
	_roomParent = roomGenerator;
}

ARoomGenerator* ATile::GetTileParentRoom()
{
	return  _roomParent;
}

void ATile::SetPositionInRoom(int row, int column)
{
	_row = row;
	_column = column;
}

bool ATile::IsTileMarked()
{
	return _isInteractible;
}

void ATile::MarkTileInteractible()
{
	_isInteractible = true;
	SetOutlineStatus(true);
}

void ATile::ClearTileMarkedStatus()
{
	_isInteractible = false;
	SetOutlineStatus(false);
}

int ATile::GetRow()
{
	return _row;
}

int ATile::GetColumn()
{
	return  _column;
}

void ATile::SetTileType(TileType tileType)
{
	_tileType = tileType;

	switch (tileType)
	{
	case TileType::LavaTile:
		SetTileMaterial(LavaMaterial);
		break;

	case TileType::VictoryTile:
		SetTileMaterial(VictoryMaterial);
		break;

	case TileType::UpgradeTile:
		SetTileMaterial(UpgradeMaterial);
		break;

	case TileType::FloorTile:
	default:
		SetTileMaterial(FloorMaterial);
		break;
	}
}

TileType ATile::GetTileType()
{
	return  _tileType;
}

void ATile::SetPickupType(PickupType pickupType)
{
	if (_pickupItem != nullptr)
	{
		_pickupItem->Destroy();
	}

	AActor* pickup = nullptr;
	switch (pickupType)
	{
	case PickupType::None:
		break;

	case PickupType::Spear:
		pickup = GetWorld()->SpawnActor(Pickups[pickupType], &TileCenter, &FRotator::ZeroRotator);
		break;

	default:
		break;
	}

	_pickupItem = pickup;
}

void ATile::ClearPickup()
{
	SetPickupType(PickupType::None);
}

PickupType ATile::GetPickupType()
{
	return  _pickupType;
}
