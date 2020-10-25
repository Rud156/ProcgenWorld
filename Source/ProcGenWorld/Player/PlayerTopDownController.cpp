// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerTopDownController.h"
#include "PlayerCharacter.h"
#include "PlayerSpawn.h"
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
		bool isTileMarked = tile->IsTileMarked();

		if (isTileMarked) {
			bool movementSuccess = _playerCharacter->HandleMouseClicked(hitResult, tile);

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
	else {
		if (hitActor != nullptr) {
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, *hitActor->GetName());
		}
	}
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
	_currentRoom->MarkValidSpots(_playerRoomRow, _playerRoomColumn);
}

void APlayerTopDownController::HandlePlayerPushAction()
{

}

void APlayerTopDownController::HandlePlayerAttackAction()
{

}

void APlayerTopDownController::HandlePlayerSpearAction()
{

}

void APlayerTopDownController::HandlePlayerJumpAction()
{

}