// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerTopDownController.h"
#include "PlayerCharacter.h"
#include "PlayerSpawn.h"
#include "UpgradeController.h"
#include "../Enemy/EnemyControllerBase.h"
#include "../Room/Tile.h"
#include "../Room/RoomGenerator.h"
#include "../Game/GameController.h"
#include "../Game/MainGameInstance.h"

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

	_playerPickups = TMap<PickupType, int>();

	auto upgradeActor = GetWorld()->SpawnActor(UpgradeController, &FVector::ZeroVector, &FRotator::ZeroRotator);
	_upgradeController = Cast<AUpgradeController>(upgradeActor);
	_upgradeController->SetPlayerController(this);

	_hoverDisplayTile = GetWorld()->SpawnActor(HoverDisplayPrefab, &FVector::ZeroVector, &FRotator::ZeroRotator);
}

// Called every frame
void APlayerTopDownController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_playerCharacter != nullptr) {
		FVector targetLocation = _playerCharacter->GetActorLocation() + FollowOffset;
		SetActorLocation(targetLocation);
	}

	UpdateHoverDisplayTile();
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
			CollectPickup(tile->GetPickupType());
			tile->ClearPickup();

			_playerRoomRow = tile->GetRow();
			_playerRoomColumn = tile->GetColumn();

			_currentRoom->ClearAllTileMarkedStatus();

			if (tile->GetTileType() == TileType::UpgradeTile)
			{
				_gameController->ResetPlayerTurnTime();
				_upgradeController->ShowRandomUpgrades();
			}
			else if (tile->GetTileType() == TileType::VictoryTile)
			{
				auto gameInstance = UGameplayStatics::GetGameInstance(GetWorld());
				auto mainGameInstance = Cast<UMainGameInstance>(gameInstance);
				mainGameInstance->DidPlayerWin = true;

				UGameplayStatics::OpenLevel(GetWorld(), EndingLevel);
			}
			else
			{
				_gameController->EndPlayerTurn();
			}
		}
	}
	else if (isTileMarked && tileData == WorldElementType::Enemy)
	{
		if (HasPickup(PickupType::Spear))
		{
			auto enemy = _currentRoom->GetEnemyAtPosition(row, column);
			if (enemy != nullptr)
			{
				enemy->TakeDamage(enemy->GetMaxHealth());
			}

			bool movementSuccess = _playerCharacter->MoveToTilePosition(hitResult, tile);
			if (movementSuccess)
			{
				CollectPickup(tile->GetPickupType());
				tile->ClearPickup();

				_playerRoomRow = tile->GetRow();
				_playerRoomColumn = tile->GetColumn();

				_currentRoom->ClearAllTileMarkedStatus();

				if (tile->GetTileType() == TileType::UpgradeTile)
				{
					_gameController->ResetPlayerTurnTime();
					_upgradeController->ShowRandomUpgrades();
				}
				else if (tile->GetTileType() == TileType::VictoryTile)
				{
					auto gameInstance = UGameplayStatics::GetGameInstance(GetWorld());
					auto mainGameInstance = Cast<UMainGameInstance>(gameInstance);
					mainGameInstance->DidPlayerWin = true;

					UGameplayStatics::OpenLevel(GetWorld(), EndingLevel);
				}
				else
				{
					_gameController->EndPlayerTurn();
				}
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

		_currentRoom->ClearAllTileMarkedStatus();
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

	_currentRoom->ClearAllTileMarkedStatus();
	_gameController->EndPlayerTurn();
}

void APlayerTopDownController::ExecuteSpearThrowAction(ATile* tile)
{
	if (!tile->IsTileMarked() || !HasPickup(PickupType::Spear))
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

		UsePickup(PickupType::Spear);
		tile->SetPickupType(PickupType::Spear);
	}

	_currentRoom->ClearAllTileMarkedStatus();
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

			CollectPickup(tile->GetPickupType());
			tile->ClearPickup();

			_playerRoomRow = tile->GetRow();
			_playerRoomColumn = tile->GetColumn();

			_currentRoom->ClearAllTileMarkedStatus();

			if (tile->GetTileType() == TileType::UpgradeTile)
			{
				_gameController->ResetPlayerTurnTime();
				_upgradeController->ShowRandomUpgrades();
			}
			else if (tile->GetTileType() == TileType::VictoryTile)
			{
				auto gameInstance = UGameplayStatics::GetGameInstance(GetWorld());
				auto mainGameInstance = Cast<UMainGameInstance>(gameInstance);
				mainGameInstance->DidPlayerWin = true;

				UGameplayStatics::OpenLevel(GetWorld(), EndingLevel);
			}
			else
			{
				_gameController->EndPlayerTurn();
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Invalid Tile For Jump!!!");
	}
}

void APlayerTopDownController::UpdateHoverDisplayTile()
{
	if (_hoverDisplayTile != nullptr)
	{
		APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

		FHitResult hitResult;
		playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, hitResult);

		auto hitActor = hitResult.GetActor();
		ATile* tile = Cast<ATile>(hitActor);

		if (tile != nullptr)
		{
			_hoverDisplayTile->SetActorLocation(tile->TileCenter + HoverDisplayOffset);
		}
		else
		{
			_hoverDisplayTile->SetActorLocation(HoverDisplayDefaultPosition);
		}
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
		else if (tile->GetTileType() == TileType::VictoryTile)
		{
			auto gameInstance = UGameplayStatics::GetGameInstance(GetWorld());
			auto mainGameInstance = Cast<UMainGameInstance>(gameInstance);
			mainGameInstance->DidPlayerWin = true;

			UGameplayStatics::OpenLevel(GetWorld(), EndingLevel);
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

	auto gameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	auto mainGameInstance = Cast<UMainGameInstance>(gameInstance);
	mainGameInstance->DidPlayerWin = false;

	UGameplayStatics::OpenLevel(GetWorld(), EndingLevel);
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

	if (_lastClickedTile->GetTileType() == TileType::UpgradeTile)
	{
		_lastClickedTile->SetTileType(TileType::FloorTile);
	}

	_gameController->EndPlayerTurn();
}

void APlayerTopDownController::CollectPickup(PickupType pickupType, int count)
{
	int currentCount = 0;
	if (_playerPickups.Contains(pickupType))
	{
		currentCount = _playerPickups[pickupType];
	}

	currentCount += count;
	_playerPickups.Add(pickupType, currentCount);

	FMyPickupType myPickup = FMyPickupType();
	myPickup.Pickups = _playerPickups;
	OnPickupsChanged.Broadcast(myPickup);
}

void APlayerTopDownController::UsePickup(PickupType pickupType)
{
	if (_playerPickups.Contains(pickupType))
	{
		int count = _playerPickups[pickupType];
		count -= 1;

		if (count <= 0)
		{
			_playerPickups.Remove(pickupType);
		}
		else
		{
			_playerPickups.Add(pickupType, count);
		}
	}

	FMyPickupType myPickup = FMyPickupType();
	myPickup.Pickups = _playerPickups;
	OnPickupsChanged.Broadcast(myPickup);
}

bool APlayerTopDownController::HasPickup(PickupType pickupType)
{
	if (_playerPickups.Contains(pickupType) && _playerPickups[pickupType] > 0)
	{
		return true;
	}

	return false;
}

TMap<PickupType, int> APlayerTopDownController::GetPickups()
{
	return _playerPickups;
}

void APlayerTopDownController::HandlePlayerMoveAction()
{
	_currentRoom->ClearAllTileMarkedStatus();

	_currentRoom->MarkAdjacentMovementSpots(_playerRoomRow, _playerRoomColumn);
	_lastPlayerAction = ActionType::Move;
}

void APlayerTopDownController::HandlePlayerPushAction()
{
	_currentRoom->ClearAllTileMarkedStatus();
	auto worldState = _currentRoom->GetWorldState();

	int leftSide = _playerRoomColumn - 1;
	int rightSide = _playerRoomColumn + 1;
	int topSide = _playerRoomRow - 1;
	int bottomSide = _playerRoomRow + 1;

	int floorRows = _currentRoom->GetRowCount() - 2;
	int floorColumns = _currentRoom->GetColumnCount();

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
	_currentRoom->ClearAllTileMarkedStatus();

	auto worldState = _currentRoom->GetWorldState();

	int leftSide = _playerRoomColumn - 1;
	int rightSide = _playerRoomColumn + 1;
	int topSide = _playerRoomRow - 1;
	int bottomSide = _playerRoomRow + 1;

	int floorRows = _currentRoom->GetRowCount() - 2;
	int floorColumns = _currentRoom->GetColumnCount();

	_currentRoom->ClearAllTileMarkedStatus();

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
	_currentRoom->ClearAllTileMarkedStatus();

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
	_currentRoom->ClearAllTileMarkedStatus();

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