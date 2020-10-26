// Fill out your copyright notice in the Description page of Project Settings.


#include "GameController.h"
#include "../Player/PlayerTopDownController.h"
#include "../Enemy/EnemyControllerBase.h"
#include "../Room/RoomGenerator.h"

// Sets default values
AGameController::AGameController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerTurnWaitTime = 10;
	AITurnWaitTime = 3;
}

// Called when the game starts or when spawned
void AGameController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Hello World!!!"));
}

// Called every frame
void AGameController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!_gameTurnActive)
	{
		return;
	}

	if (_currentTurnTime > 0)
	{
		_currentTurnTime -= DeltaTime;

		if (_currentTurnTime <= 0) {
			if (_isPlayerTurn) {
				EndPlayerTurn();
			}
			else {
				BeginPlayerTurn();
			}
		}
	}
}

void AGameController::SetPlayerTopDownController(APlayerTopDownController* playerTopDownController)
{
	_playerTopDownController = playerTopDownController;

	BeginPlayerTurn();
}

void AGameController::SetCurrentRoom(ARoomGenerator* currentRoom)
{
	_currentRoom = currentRoom;
	_currentRoom->OnRoomCleared.AddDynamic(this, &AGameController::HandleRoomCleared);
}

void AGameController::HandleRoomCleared()
{
	_currentRoom->OnRoomCleared.RemoveDynamic(this, &AGameController::HandleRoomCleared);
	EndGameTurn();
}

void AGameController::BeginGameTurn()
{
	_gameTurnActive = true;
	_currentRoom->CheckAndActivateRoom();
	_playerTopDownController->DisableFreeMovement();

	BeginPlayerTurn();
}

void AGameController::EndGameTurn()
{
	_gameTurnActive = false;
	_playerTopDownController->DisablePlayerTurn();
	_playerTopDownController->EnableFreeMovement();
}

void AGameController::BeginPlayerTurn()
{
	_isPlayerTurn = true;
	_currentTurnTime = PlayerTurnWaitTime;

	_playerTopDownController->EnablePlayerTurn();
}

void AGameController::EndPlayerTurn()
{
	_isPlayerTurn = false;
	_currentTurnTime = AITurnWaitTime;

	_currentRoom->ClearAllTilesStatus();
	_playerTopDownController->DisablePlayerTurn();

	ExecuteEnemyAI();
}

void AGameController::ExecuteEnemyAI()
{
	auto enemies = _currentRoom->GetEnemies();
	for (int i = 0; i < enemies.Num(); i++)
	{
		enemies[i]->Execute();
	}
}

bool AGameController::GetPlayerTurnStatus()
{
	return _isPlayerTurn;
}

float AGameController::GetTurnTime()
{
	return _currentTurnTime;
}

