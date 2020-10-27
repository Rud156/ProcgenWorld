// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerTopDownController.h"
#include "PlayerCharacter.h"
#include "PlayerSpawn.h"
#include "UpgradeController.h"
#include "../Enemy/EnemyControllerBase.h"
#include "../Room/Tile.h"
#include "../Room/RoomGenerator.h"
#include "../Game/GameController.h"

#include "Components/SceneComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerTopDownController::APlayerTopDownController()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TopDownSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TopDownSceneComponent"));
	RootComponent = TopDownSceneComponent;

	MaxHealth = 3;
	MaxMana = 5;

	MeleeDamageAmount = 1;

	EnemyPushDamage = 1;
	EnemyPushManaCost = 3;

	MinSpearRadius = 2;
	MaxSpearRadius = 3;

	MinDashRadius = 2;
	MaxDashRadius = 3;
	DashManaCost = 3;

	DefaultMovementSpeed = 600;
	DashMovementSpeed = 1200;
}

// Called when the game starts or when spawned
void APlayerTopDownController::BeginPlay()
{
	Super::BeginPlay();

	_currentHP = MaxHealth;
	_currentMana = MaxMana;
	_maxHP = MaxHealth;
	_maxMana = MaxMana;

	_minSpearRadius = MinSpearRadius;
	_maxSpearRadius = MaxSpearRadius;

	_minJumpRadius = MinDashRadius;
	_maxJumpRadius = MaxDashRadius;

	auto upgradeActor = GetWorld()->SpawnActor(UpgradeController, &FVector::ZeroVector, &FRotator::ZeroRotator);
	_upgradeController = Cast<AUpgradeController>(upgradeActor);
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
	PlayerInputComponent->BindAction("Key1", EInputEvent::IE_Pressed, this, &APlayerTopDownController::Handle1Pressed);
	PlayerInputComponent->BindAction("Key2", EInputEvent::IE_Pressed, this, &APlayerTopDownController::Handle2Pressed);
}

void APlayerTopDownController::HandleMouseClicked()
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Mouse Clicked");
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	FHitResult hitResult;
	playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, hitResult);

	auto hitActor = hitResult.GetActor();
	ATile* tile = Cast<ATile>(hitActor);

	if (tile != nullptr) {
		_lastClickedTile = tile;

		if (_isPlayerTurn)
		{
			switch (_lastPlayerAction)
			{
			case ActionType::Move:
				ExecuteMoveToTileAction(hitResult, tile);
				break;

			case ActionType::Jump:
				ExecuteDashAction(hitResult, tile);
				break;

			case ActionType::Attack:
				ExecuteAttackTileAction(tile);
				break;

			case ActionType::SpearThrow:
				ExecuteSpearThrowAction(tile);
				break;

			case ActionType::Push:
				ExecutePushAction(tile);
				break;
			}
		}
		else if (_hasFreeMovement)
		{
			_playerCharacter->MoveToTilePosition(hitResult, tile);
		}
	}
	else {
		if (hitActor != nullptr) {
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, *hitActor->GetName());
		}
	}
}

void APlayerTopDownController::Handle1Pressed()
{
	ResetPlayerHealth();
	ResetPlayerMana();
}

void APlayerTopDownController::Handle2Pressed()
{
	if (_currentRoom != nullptr)
	{
		_currentRoom->ClearAllEnemies();
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
			if (tile->GetPickupType() == PickupType::Spear) // TODO: Change this later on...
			{
				tile->SetPickupType(PickupType::None);
				_playerHasSpear = true;
			}

			_playerRoomRow = tile->GetRow();
			_playerRoomColumn = tile->GetColumn();

			_currentRoom->ClearAllTilesStatus();
			_gameController->EndPlayerTurn();
		}
	}
	else if (isTileMarked && tileData == WorldElementType::Enemy)
	{
		if (_playerHasSpear)
		{
			auto enemy = _currentRoom->GetEnemyAtPosition(row, column);
			if (enemy != nullptr)
			{
				enemy->TakeDamage(enemy->GetMaxHealth());
			}

			bool movementSuccess = _playerCharacter->MoveToTilePosition(hitResult, tile);
			if (movementSuccess)
			{
				if (tile->GetPickupType() == PickupType::Spear) // TODO: Change this later on...
				{
					tile->SetPickupType(PickupType::None);
					_playerHasSpear = true;
				}

				_playerRoomRow = tile->GetRow();
				_playerRoomColumn = tile->GetColumn();

				_currentRoom->ClearAllTilesStatus();
				_gameController->EndPlayerTurn();
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Invalid Tile For Movement!!!");
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

	if (!tile->IsTileMarked())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Invalid Tile For Push!!!");
		return;
	}

	auto worldState = _currentRoom->GetWorldState();

	int leftSide = _playerRoomColumn - 1;
	int rightSide = _playerRoomColumn + 1;
	int topSide = _playerRoomRow - 1;
	int bottomSide = _playerRoomRow + 1;

	// Left Side
	if (row == _playerRoomRow && column == leftSide)
	{
		TArray<AEnemyControllerBase*> enemyInRow = TArray<AEnemyControllerBase*>();
		TileType lastTileType = TileType::WallTile;

		for (int i = leftSide; i >= 0; i--)
		{
			auto tileData = worldState[row][i];
			if (tileData == WorldElementType::Enemy)
			{
				enemyInRow.Add(_currentRoom->GetEnemyAtPosition(row, i));
			}
			else if (tileData == WorldElementType::Floor)
			{
				lastTileType = TileType::FloorTile;
				break;
			}
			else if (tileData == WorldElementType::LavaTile)
			{
				lastTileType = TileType::LavaTile;
				break;
			}
		}

		switch (lastTileType)
		{
		case TileType::WallTile:
		{
			for (int i = 0; i < enemyInRow.Num(); i++)
			{
				enemyInRow[i]->TakeDamage(EnemyPushDamage);
			}
		}
		break;

		case TileType::FloorTile:
		{
			for (int i = 0; i < enemyInRow.Num(); i++)
			{
				int currentRow = enemyInRow[i]->GetRow();
				int currentColumn = enemyInRow[i]->GetColumn();

				enemyInRow[i]->Move(currentRow, currentColumn - 1);
			}
		}
		break;

		case TileType::LavaTile:
		{
			for (int i = 0; i < enemyInRow.Num(); i++)
			{
				if (i == enemyInRow.Num() - 1)
				{
					enemyInRow[i]->TakeDamage(enemyInRow[i]->GetMaxHealth());
				}
				else
				{
					int currentRow = enemyInRow[i]->GetRow();
					int currentColumn = enemyInRow[i]->GetColumn();

					enemyInRow[i]->Move(currentRow, currentColumn - 1);
				}
			}
		}
		break;

		default:
			// Do Nothing Here...
			break;
		}
	}
	// Right Side
	else if (row == _playerRoomRow && column == rightSide)
	{
		TArray<AEnemyControllerBase*> enemyInRow = TArray<AEnemyControllerBase*>();
		TileType lastTileType = TileType::WallTile;

		int floorColumns = _currentRoom->GetColumnCount();

		for (int i = rightSide; i <= floorColumns; i++)
		{
			auto tileData = worldState[row][i];
			if (tileData == WorldElementType::Enemy)
			{
				enemyInRow.Add(_currentRoom->GetEnemyAtPosition(row, i));
			}
			else if (tileData == WorldElementType::Floor)
			{
				lastTileType = TileType::FloorTile;
				break;
			}
			else if (tileData == WorldElementType::LavaTile)
			{
				lastTileType = TileType::LavaTile;
				break;
			}
		}

		switch (lastTileType)
		{
		case TileType::WallTile:
		{
			for (int i = 0; i < enemyInRow.Num(); i++)
			{
				enemyInRow[i]->TakeDamage(EnemyPushDamage);
			}
		}
		break;

		case TileType::FloorTile:
		{
			for (int i = 0; i < enemyInRow.Num(); i++)
			{
				int currentRow = enemyInRow[i]->GetRow();
				int currentColumn = enemyInRow[i]->GetColumn();

				enemyInRow[i]->Move(currentRow, currentColumn + 1);
			}
		}
		break;

		case TileType::LavaTile:
		{
			for (int i = 0; i < enemyInRow.Num(); i++)
			{
				if (i == enemyInRow.Num() - 1)
				{
					enemyInRow[i]->TakeDamage(enemyInRow[i]->GetMaxHealth());
				}
				else
				{
					int currentRow = enemyInRow[i]->GetRow();
					int currentColumn = enemyInRow[i]->GetColumn();

					enemyInRow[i]->Move(currentRow, currentColumn + 1);
				}
			}
		}
		break;

		default:
			// Do Nothing Here...
			break;
		}
	}
	// Top Side
	else if (row == topSide && column == _playerRoomColumn)
	{
		TArray<AEnemyControllerBase*> enemyInRow = TArray<AEnemyControllerBase*>();
		TileType lastTileType = TileType::WallTile;

		for (int i = topSide; i >= 0; i--)
		{
			auto tileData = worldState[i][column];
			if (tileData == WorldElementType::Enemy)
			{
				enemyInRow.Add(_currentRoom->GetEnemyAtPosition(i, column));
			}
			else if (tileData == WorldElementType::Floor)
			{
				lastTileType = TileType::FloorTile;
				break;
			}
			else if (tileData == WorldElementType::LavaTile)
			{
				lastTileType = TileType::LavaTile;
				break;
			}
		}

		switch (lastTileType)
		{
		case TileType::WallTile:
		{
			for (int i = 0; i < enemyInRow.Num(); i++)
			{
				enemyInRow[i]->TakeDamage(EnemyPushDamage);
			}
		}
		break;

		case TileType::FloorTile:
		{
			for (int i = 0; i < enemyInRow.Num(); i++)
			{
				int currentRow = enemyInRow[i]->GetRow();
				int currentColumn = enemyInRow[i]->GetColumn();

				enemyInRow[i]->Move(currentRow - 1, currentColumn);
			}
		}
		break;

		case TileType::LavaTile:
		{
			for (int i = 0; i < enemyInRow.Num(); i++)
			{
				if (i == enemyInRow.Num() - 1)
				{
					enemyInRow[i]->TakeDamage(enemyInRow[i]->GetMaxHealth());
				}
				else
				{
					int currentRow = enemyInRow[i]->GetRow();
					int currentColumn = enemyInRow[i]->GetColumn();

					enemyInRow[i]->Move(currentRow - 1, currentColumn);
				}
			}
		}
		break;

		default:
			// Do Nothing Here...
			break;
		}
	}
	// Bottom Side
	else if (row == bottomSide && column == _playerRoomColumn)
	{
		TArray<AEnemyControllerBase*> enemyInRow = TArray<AEnemyControllerBase*>();
		TileType lastTileType = TileType::WallTile;

		int floorRows = _currentRoom->GetRowCount() - 2;

		for (int i = bottomSide; i <= floorRows; i++)
		{
			auto tileData = worldState[i][column];
			if (tileData == WorldElementType::Enemy)
			{
				enemyInRow.Add(_currentRoom->GetEnemyAtPosition(i, column));
			}
			else if (tileData == WorldElementType::Floor)
			{
				lastTileType = TileType::FloorTile;
				break;
			}
			else if (tileData == WorldElementType::LavaTile)
			{
				lastTileType = TileType::LavaTile;
				break;
			}
		}

		switch (lastTileType)
		{
		case TileType::WallTile:
		{
			for (int i = 0; i < enemyInRow.Num(); i++)
			{
				enemyInRow[i]->TakeDamage(EnemyPushDamage);
			}
		}
		break;

		case TileType::FloorTile:
		{
			for (int i = 0; i < enemyInRow.Num(); i++)
			{
				int currentRow = enemyInRow[i]->GetRow();
				int currentColumn = enemyInRow[i]->GetColumn();

				enemyInRow[i]->Move(currentRow + 1, currentColumn);
			}
		}
		break;

		case TileType::LavaTile:
		{
			for (int i = 0; i < enemyInRow.Num(); i++)
			{
				if (i == enemyInRow.Num() - 1)
				{
					enemyInRow[i]->TakeDamage(enemyInRow[i]->GetMaxHealth());
				}
				else
				{
					int currentRow = enemyInRow[i]->GetRow();
					int currentColumn = enemyInRow[i]->GetColumn();

					enemyInRow[i]->Move(currentRow + 1, currentColumn);
				}
			}
		}
		break;

		default:
			// Do Nothing Here...
			break;
		}
	}

	_currentRoom->ClearAllTilesStatus();
	_gameController->EndPlayerTurn();
}

void APlayerTopDownController::ExecuteSpearThrowAction(ATile* tile)
{
	if (!tile->IsTileMarked() || !_playerHasSpear)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Invalid Spear Throw");
		return;
	}

	int row = tile->GetRow();
	int column = tile->GetColumn();

	AEnemyControllerBase* enemy = _currentRoom->GetEnemyAtPosition(row, column);
	if (enemy != nullptr)
	{
		enemy->TakeDamage(enemy->GetMaxHealth());
		_playerHasSpear = false;

		tile->SetPickupType(PickupType::Spear);
	}

	_currentRoom->ClearAllTilesStatus();
	_gameController->EndPlayerTurn();
}

void APlayerTopDownController::ExecuteDashAction(FHitResult hitResult, ATile* tile)
{
	if (!HasMana(DashManaCost))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Not Enough Mana");
		return;
	}

	bool isTileMarked = tile->IsTileMarked();
	auto worldStatus = _currentRoom->GetWorldState();

	int row = tile->GetRow();
	int column = tile->GetColumn();
	auto tileData = worldStatus[row][column];

	if (isTileMarked && tileData == WorldElementType::Floor) {
		_playerCharacter->GetCharacterMovement()->MaxWalkSpeed = DashMovementSpeed;
		bool movementSuccess = _playerCharacter->MoveToTilePosition(hitResult, tile);

		if (movementSuccess)
		{
			UseMana(DashManaCost);

			if (tile->GetPickupType() == PickupType::Spear) // TODO: Change this later on...
			{
				tile->SetPickupType(PickupType::None);
				_playerHasSpear = true;
			}

			_playerRoomRow = tile->GetRow();
			_playerRoomColumn = tile->GetColumn();

			_currentRoom->ClearAllTilesStatus();
			_gameController->EndPlayerTurn();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Invalid Tile For Jump!!!");
	}
}

void APlayerTopDownController::SetDefaultProperties(APlayerCharacter* playerCharacter, AGameController* gameController)
{
	_playerCharacter = playerCharacter;
	_gameController = gameController;

	_playerCharacter->OnPlayerReachedPosition.AddDynamic(this, &APlayerTopDownController::HandlePlayerReachedPosition);
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
	_upgradeController->ClearUpgradesUI();
}

bool APlayerTopDownController::GetIsPlayerTurn()
{
	return _isPlayerTurn;
}

void APlayerTopDownController::EnableFreeMovement()
{
	_hasFreeMovement = true;
}

void APlayerTopDownController::DisableFreeMovement()
{
	_hasFreeMovement = false;
}

ARoomGenerator* APlayerTopDownController::GetRoomInstance()
{
	return _currentRoom;
}

int APlayerTopDownController::GetPlayerRow()
{
	return _playerRoomRow;
}

int APlayerTopDownController::GetPlayerColumn()
{
	return _playerRoomColumn;
}

void APlayerTopDownController::HandlePlayerReachedPosition()
{
	_playerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMovementSpeed;

	if (_hasFreeMovement)
	{
		ATile* tile = _lastClickedTile;

		_playerRoomRow = tile->GetRow();
		_playerRoomColumn = tile->GetColumn();

		auto tileParent = tile->GetTileParentRoom();

		ARoomGenerator* parentRoom = tile->GetTileParentRoom();
		if (!parentRoom->IsRoomCleared())
		{
			SetCurrentRoom(parentRoom);
			ResetPlayerMana();

			_gameController->SetCurrentRoom(parentRoom);
			_gameController->BeginGameTurn();
		}
	}
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

void APlayerTopDownController::ApplyUpgrade(UpgradeType upgradeType)
{
	switch (upgradeType)
	{
	case UpgradeType::RestoreHP:
		ResetPlayerHealth();
		break;

	case UpgradeType::IncreaseHP:
		_maxHP += 1;
		break;

	case UpgradeType::IncreaseMana:
		_maxMana += 1;
		break;

	case UpgradeType::IncreaseJump:
		_maxJumpRadius += 1;
		break;

	case UpgradeType::IncreaseSpear:
		_maxSpearRadius += 1;
		break;

	default:
		break;
	}
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
	int minTopLeftRow = _playerRoomRow - _minSpearRadius;
	int minTopLeftColumn = _playerRoomColumn - _minSpearRadius;
	int minBottomRightRow = _playerRoomRow + _minSpearRadius;
	int minBottomRightColumn = _playerRoomColumn + _minSpearRadius;

	int maxTopLeftRow = _playerRoomRow - _maxSpearRadius;
	int maxTopLeftColumn = _playerRoomColumn - _maxSpearRadius;
	int maxBottomRightRow = _playerRoomRow + _maxSpearRadius;
	int maxBottomRightColumn = _playerRoomColumn + _maxSpearRadius;

	for (int i = maxTopLeftRow; i <= maxBottomRightRow; i++)
	{
		for (int j = maxTopLeftColumn; j <= maxBottomRightColumn; j++)
		{
			if (((i <= minTopLeftRow || i >= minBottomRightRow) ||
				(j <= minTopLeftColumn || j >= minBottomRightColumn)) &&
				_currentRoom->IsPositionInRoom(i, j))
			{
				_currentRoom->MarkTile(i, j);
			}
		}
	}

	_lastPlayerAction = ActionType::SpearThrow;
}

void APlayerTopDownController::HandlePlayerDashAction()
{
	int minTopLeftRow = _playerRoomRow - _minJumpRadius;
	int minTopLeftColumn = _playerRoomColumn - _minJumpRadius;
	int minBottomRightRow = _playerRoomRow + _minJumpRadius;
	int minBottomRightColumn = _playerRoomColumn + _minJumpRadius;

	int maxTopLeftRow = _playerRoomRow - _maxJumpRadius;
	int maxTopLeftColumn = _playerRoomColumn - _maxJumpRadius;
	int maxBottomRightRow = _playerRoomRow + _maxJumpRadius;
	int maxBottomRightColumn = _playerRoomColumn + _maxJumpRadius;

	for (int i = maxTopLeftRow; i <= maxBottomRightRow; i++)
	{
		for (int j = maxTopLeftColumn; j <= maxBottomRightColumn; j++)
		{
			if (((i <= minTopLeftRow || i >= minBottomRightRow) ||
				(j <= minTopLeftColumn || j >= minBottomRightColumn)) &&
				_currentRoom->IsPositionInRoom(i, j))
			{
				_currentRoom->MarkTile(i, j);
			}
		}
	}

	_lastPlayerAction = ActionType::Jump;
}