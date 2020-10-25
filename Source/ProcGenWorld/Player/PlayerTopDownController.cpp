// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerTopDownController.h"
#include "PlayerCharacter.h"
#include "PlayerSpawn.h"
#include "../Enemy/EnemyControllerBase.h"
#include "../Room/Tile.h"
#include "../Room/RoomGenerator.h"
#include "../Game/GameController.h"

#include "Components/SceneComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

// Sets default values
APlayerTopDownController::APlayerTopDownController()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TopDownSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TopDownSceneComponent"));
	RootComponent = TopDownSceneComponent;

	MaxHealth = 3;
	MaxMana = 5;

}

// Called when the game starts or when spawned
void APlayerTopDownController::BeginPlay()
{
	Super::BeginPlay();

	_currentHP = MaxHealth;
	_currentMana = MaxMana;
	_maxHP = MaxHealth;
	_maxMana = MaxMana;
}

// Called every frame
void APlayerTopDownController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_playerCharacter != nullptr) {
		FVector targetLocation = _playerCharacter->GetActorLocation() + FollowOffset;
		SetActorLocation(targetLocation);
	}
}

// Called to bind functionality to input
void APlayerTopDownController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("LeftClick", EInputEvent::IE_Pressed, this, &APlayerTopDownController::HandleMouseClicked);
}

void APlayerTopDownController::HandleMouseClicked()
{
	if (!_isPlayerTurn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Not Player's Turn");
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Mouse Clicked");
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	FHitResult hitResult;
	playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, hitResult);

	auto hitActor = hitResult.GetActor();
	ATile* tile = Cast<ATile>(hitActor);

	if (tile != nullptr) {
		switch (_lastPlayerAction)
		{
		case ActionType::Move:
			ExecuteMoveToTileAction(hitResult, tile);
			break;

		case ActionType::Jump:
			break;

		case ActionType::Attack:
			ExecuteAttackTileAction(tile);
			break;

		case ActionType::SpearThrow:
			break;

		case ActionType::Push:
			break;
		}
	}
	else {
		if (hitActor != nullptr) {
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, *hitActor->GetName());
		}
	}
}

void APlayerTopDownController::ExecuteMoveToTileAction(FHitResult hitResult, ATile* tile)
{
	bool isTileMarked = tile->IsTileMarked();
	auto worldStatus = _currentRoom->GetWorldState();

	int row = tile->GetRow();
	int column = tile->GetColumn();
	auto tileData = worldStatus[row][column];

	if (isTileMarked && tileData == WorldElementType::Floor) {
		bool movementSuccess = _playerCharacter->MoveToTilePosition(hitResult, tile);

		if (movementSuccess)
		{
			_playerRoomRow = tile->GetRow();
			_playerRoomColumn = tile->GetColumn();

			_currentRoom->ClearAllTilesStatus();
			_gameController->EndPlayerTurn();
		}
	}
	else if (isTileMarked && tileData == WorldElementType::Enemy)
	{
		bool movementSuccess = _playerCharacter->MoveToTilePosition(hitResult, tile);
		if (_playerHasSpear)
		{
			auto enemy = _currentRoom->GetEnemyAtPosition(row, column);
			if (enemy != nullptr)
			{
				enemy->TakeDamage(SpearDamageAmount);
			}
		}

		if (movementSuccess)
		{
			_playerRoomRow = tile->GetRow();
			_playerRoomColumn = tile->GetColumn();

			_currentRoom->ClearAllTilesStatus();
			_gameController->EndPlayerTurn();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Invalid Tile Clicked!!!");
	}
}

void APlayerTopDownController::ExecuteAttackTileAction(ATile* tile)
{
	int row = tile->GetRow();
	int column = tile->GetColumn();
	auto enemy = _currentRoom->GetEnemyAtPosition(row, column);

	if (enemy != nullptr)
	{
		enemy->TakeDamage(MeleeDamageAmount);

		_currentRoom->ClearAllTilesStatus();
		_gameController->EndPlayerTurn();
	}
}

void APlayerTopDownController::ExecutePushAction(ATile* tile)
{
	int row = tile->GetRow();
	int column = tile->GetColumn();
}

void APlayerTopDownController::ExecuteSpearThrowAction(ATile* tile)
{
	// TODO: Implement this function...
}

void APlayerTopDownController::ExecuteJumpAction(ATile* tile)
{
	// TODO: Implement this function...
}


void APlayerTopDownController::SetDefaultProperties(APlayerCharacter* playerCharacter, AGameController* gameController)
{
	_playerCharacter = playerCharacter;
	_gameController = gameController;
}

void APlayerTopDownController::SetCurrentRoom(ARoomGenerator* roomGenerator)
{
	_currentRoom = roomGenerator;
}

void APlayerTopDownController::SetPlayerRowAndColumn(int row, int column)
{
	_playerRoomRow = row;
	_playerRoomColumn = column;
}

void APlayerTopDownController::EnablePlayerTurn()
{
	_isPlayerTurn = true;
}

void APlayerTopDownController::DisablePlayerTurn()
{
	_isPlayerTurn = false;
}

bool APlayerTopDownController::GetIsPlayerTurn()
{
	return _isPlayerTurn;
}


int APlayerTopDownController::GetPlayerRow()
{
	return _playerRoomRow;
}

int APlayerTopDownController::GetPlayerColumn()
{
	return _playerRoomColumn;
}

int APlayerTopDownController::GetPlayerHealth()
{
	return  _currentHP;
}

int APlayerTopDownController::GetPlayerMaxHealth()
{
	return _maxHP;
}

void APlayerTopDownController::ResetPlayerHealth()
{
	_currentHP = _maxHP;
}

void APlayerTopDownController::IncreasePlayerHealth(int amount)
{
	_maxHP += amount;
}

void APlayerTopDownController::TakeDamage(int damageAmount)
{
	_currentHP -= damageAmount;
	if (_currentHP <= 0)
	{
		HandlePlayerDied();
	}
}

void APlayerTopDownController::HandlePlayerDied()
{
	OnPlayerDied.Broadcast();
}

int APlayerTopDownController::GetPlayerMana()
{
	return _currentMana;
}

int APlayerTopDownController::GetPlayerMaxMana()
{
	return _maxMana;
}

void APlayerTopDownController::ResetPlayerMana()
{
	_currentMana = _maxMana;
}

void APlayerTopDownController::IncreasePlayerMana(int amount)
{
	_maxMana += amount;
}

bool APlayerTopDownController::HasMana(int manaAmount)
{
	return _currentMana >= manaAmount;
}

void APlayerTopDownController::UseMana(int manaAmount)
{
	_currentMana -= manaAmount;
}

void APlayerTopDownController::HandlePlayerMoveAction()
{
	_currentRoom->MarkAdjacentMovementSpots(_playerRoomRow, _playerRoomColumn);
	_lastPlayerAction = ActionType::Move;
}

void APlayerTopDownController::HandlePlayerPushAction()
{
	auto worldState = _currentRoom->GetWorldState();

	int leftSide = _playerRoomColumn - 1;
	int rightSide = _playerRoomColumn + 1;
	int topSide = _playerRoomRow - 1;
	int bottomSide = _playerRoomRow + 1;

	int floorRows = _currentRoom->GetRowCount() - 2;
	int floorColumns = _currentRoom->GetColumnCount();

	_currentRoom->ClearAllTilesStatus();

	if (leftSide >= 0 && worldState[_playerRoomRow][leftSide] == WorldElementType::Enemy)
	{
		_currentRoom->MarkTile(_playerRoomRow, leftSide);
	}
	if (rightSide <= floorColumns && worldState[_playerRoomRow][rightSide] == WorldElementType::Enemy)
	{
		_currentRoom->MarkTile(_playerRoomRow, rightSide);
	}
	if (topSide >= 0 && worldState[topSide][_playerRoomColumn] == WorldElementType::Enemy)
	{
		_currentRoom->MarkTile(topSide, _playerRoomColumn);
	}
	if (bottomSide <= floorRows && worldState[bottomSide][_playerRoomColumn] == WorldElementType::Enemy)
	{
		_currentRoom->MarkTile(bottomSide, _playerRoomColumn);
	}

	_lastPlayerAction = ActionType::Push;
}

void APlayerTopDownController::HandlePlayerAttackAction()
{
	auto worldState = _currentRoom->GetWorldState();

	int leftSide = _playerRoomColumn - 1;
	int rightSide = _playerRoomColumn + 1;
	int topSide = _playerRoomRow - 1;
	int bottomSide = _playerRoomRow + 1;

	int floorRows = _currentRoom->GetRowCount() - 2;
	int floorColumns = _currentRoom->GetColumnCount();

	_currentRoom->ClearAllTilesStatus();

	if (leftSide >= 0 && worldState[_playerRoomRow][leftSide] == WorldElementType::Enemy)
	{
		_currentRoom->MarkTile(_playerRoomRow, leftSide);
	}
	if (rightSide <= floorColumns && worldState[_playerRoomRow][rightSide] == WorldElementType::Enemy)
	{
		_currentRoom->MarkTile(_playerRoomRow, rightSide);
	}
	if (topSide >= 0 && worldState[topSide][_playerRoomColumn] == WorldElementType::Enemy)
	{
		_currentRoom->MarkTile(topSide, _playerRoomColumn);
	}
	if (bottomSide <= floorRows && worldState[bottomSide][_playerRoomColumn] == WorldElementType::Enemy)
	{
		_currentRoom->MarkTile(bottomSide, _playerRoomColumn);
	}

	_lastPlayerAction = ActionType::Attack;
}

void APlayerTopDownController::HandlePlayerSpearAction()
{
	// TODO: Implement this function...
	_lastPlayerAction = ActionType::SpearThrow;
}

void APlayerTopDownController::HandlePlayerJumpAction()
{
	// TODO: Implement this function...
	_lastPlayerAction = ActionType::Jump;
}