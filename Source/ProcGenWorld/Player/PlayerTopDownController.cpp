// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerTopDownController.h"
#include "PlayerCharacter.h"
#include "PlayerSpawn.h"
#include "../Room/Tile.h"
#include "../Room/RoomGenerator.h"

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
}

// Called when the game starts or when spawned
void APlayerTopDownController::BeginPlay()
{
	Super::BeginPlay();
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
				_currentRoom->MarkValidSpots(_playerRoomRow, _playerRoomColumn);
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Invalid Tile Clicked!!!");
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, *hitActor->GetName());
	}
}

void APlayerTopDownController::SetDefaultProperties(APlayerCharacter* playerCharacter, APlayerSpawn* playerSpawn)
{
	_playerCharacter = playerCharacter;
	_playerSpawn = playerSpawn;
}

void APlayerTopDownController::SetCurrentRoom(ARoomGenerator* roomGenerator)
{
	_currentRoom = roomGenerator;
}

void APlayerTopDownController::SetPlayerRowAndColumn(int row, int column)
{
	_playerRoomRow = row;
	_playerRoomColumn = column;

	_currentRoom->MarkValidSpots(_playerRoomRow, _playerRoomColumn);
}

void APlayerTopDownController::EnablePlayerTurn()
{
	_isPlayerTurn = true;
}

void APlayerTopDownController::DisablePlayerTurn()
{
	_isPlayerTurn = false;
}
