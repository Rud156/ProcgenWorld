// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"

// Sets default values
ATile::ATile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TileParent = CreateDefaultSubobject<USceneComponent>(TEXT("TileParent"));
	RootComponent = TileParent;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	TileCenter = GetActorLocation();
	_isMoveable = false;
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ATile::IsTileMarked()
{
	return _isMoveable;
}

void ATile::MarkTileMoveable()
{
	_isMoveable = true;
	// TODO: Complete this function...
}

void ATile::ClearTileMoveableStatus()
{
	_isMoveable = false;
	// TODO: Complete this function...
}