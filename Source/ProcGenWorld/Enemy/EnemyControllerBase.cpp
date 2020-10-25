// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyControllerBase.h"
#include "../Player/PlayerTopDownController.h"
#include "../Room/RoomGenerator.h"
#include "../Room/Tile.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyControllerBase::AEnemyControllerBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MaxHealth = 1;
}

// Called when the game starts or when spawned
void AEnemyControllerBase::BeginPlay()
{
	Super::BeginPlay();

	AActor* playerActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerTopDownController::StaticClass());
	_playerTopDownController = Cast<APlayerTopDownController>(playerActor);
	if (_playerTopDownController == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Unable To Find Player Controller!!!");
	}

	_health = MaxHealth;
}

// Called every frame
void AEnemyControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyControllerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyControllerBase::SetParentRoom(ARoomGenerator* roomGenerator)
{
	_parentRoom = roomGenerator;
}

void AEnemyControllerBase::SetSpawnPosition(int row, int column)
{
	_currentRow = row;
	_currentColumn = column;
}

int AEnemyControllerBase::GetRow()
{
	return _currentRow;
}

int AEnemyControllerBase::GetColumn()
{
	return _currentColumn;
}

int AEnemyControllerBase::GetCurrentHealth()
{
	return _health;
}

int AEnemyControllerBase::GetMaxHealth()
{
	return  MaxHealth;
}

void AEnemyControllerBase::TakeDamage(int damageAmount)
{
	_health -= damageAmount;
	if (_health <= 0)
	{
		HandleUnitDied();
	}
}

void AEnemyControllerBase::HandleUnitDied()
{
	_parentRoom->HandleUnitDied(this);
}

void AEnemyControllerBase::Execute()
{
}

void AEnemyControllerBase::Move(int row, int column)
{
	ATile* targetTile = _parentRoom->GetTileAtPosition(row, column);
	FVector tileCenter = targetTile->TileCenter;

	MoveEnemyTo(tileCenter);

	_currentRow = row;
	_currentColumn = column;
}

