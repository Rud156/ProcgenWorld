// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSpawn.h"
#include "PlayerModel.h"
#include "PlayerCharacter.h"
#include "PlayerTopDownController.h"
#include "../Room/Tile.h"
#include "../Room/DungeonGenerator.h"
#include "../Room/RoomGenerator.h"
#include "../Game/GameController.h"

#include "GameFramework/DefaultPawn.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TriggerBox.h"

// Sets default values
APlayerSpawn::APlayerSpawn()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlayerSpawn::BeginPlay()
{
	Super::BeginPlay();

	AActor* triggerActor = GetWorld()->SpawnActor(TriggerBoxPrefab, &FVector::ZeroVector, &FRotator::ZeroRotator);
	_victoryTrigger = Cast<ATriggerBox>(triggerActor);
	if (_victoryTrigger == nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Trigger Box Not Found!!!");
	}

	auto defaultPawn = UGameplayStatics::GetActorOfClass(GetWorld(), ADefaultPawn::StaticClass());
	_defaultPawn = Cast<ADefaultPawn>(defaultPawn);
	if (_defaultPawn == nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Default Pawn Not Found!!!");
	}

	auto dungeonActor = UGameplayStatics::GetActorOfClass(GetWorld(), ADungeonGenerator::StaticClass());
	_dungeonGen = Cast<ADungeonGenerator>(dungeonActor);
	if (_dungeonGen != nullptr) {
		_dungeonGen->OnGenerationComplete.AddDynamic(this, &APlayerSpawn::RoomGenerationComplete);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Unable to find Dungeon Generator!!!");
	}

	auto gameControllerActor = UGameplayStatics::GetActorOfClass(GetWorld(), AGameController::StaticClass());
	_gameController = Cast<AGameController>(gameControllerActor);
	if (_gameController == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Unable To Find GameController!!!");
	}
}

// Called every frame
void APlayerSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerSpawn::RoomGenerationComplete()
{
	FTimerHandle unusedHandle;
	GetWorldTimerManager().SetTimer(unusedHandle, this, &APlayerSpawn::SpawnPlayer, PlayerSpawnDelay, false);
}

void APlayerSpawn::RoomGenerationStarted()
{
	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	controller->UnPossess();
	controller->Possess(_defaultPawn);
}

void APlayerSpawn::SpawnPlayer()
{
	if (_playerCharacter != nullptr) {
		_playerCharacter->Destroy();
	}
	if (_playerModel != nullptr) {
		_playerModel->Destroy();
	}
	if (_playerTopDownController != nullptr) {
		_playerTopDownController->Destroy();
	}

	int spawnRow = _dungeonGen->GetSpawnRow();
	int spawnColumn = _dungeonGen->GetSpawnColumn();
	ARoomGenerator* spawnRoom = _dungeonGen->GetRoom(spawnRow, spawnColumn);

	int row;
	int column;
	ATile* tile = spawnRoom->GetRandomTileInRoom(row, column);
	FVector startPosition = tile->TileCenter;

	AActor* playerModelActor = GetWorld()->SpawnActor(PlayerModelPrefab, &startPosition, &FRotator::ZeroRotator);
	AActor* playerCharacterActor = GetWorld()->SpawnActor(PlayerCharacterPrefab, &startPosition, &FRotator::ZeroRotator);
	AActor* playerTopDownControllerActor = GetWorld()->SpawnActor(PlayerTopDownControllerPrefab, &startPosition, &FRotator::ZeroRotator);

	_playerModel = Cast<APlayerModel>(playerModelActor);
	_playerCharacter = Cast<APlayerCharacter>(playerCharacterActor);
	_playerTopDownController = Cast<APlayerTopDownController>(playerTopDownControllerActor);
	
	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	controller->UnPossess();
	controller->Possess(_playerTopDownController);

	int exitRow = _dungeonGen->GetExitRow();
	int exitColumn = _dungeonGen->GetExitColumn();
	ARoomGenerator* exitRoom = _dungeonGen->GetRoom(exitRow, exitColumn);

	_victoryTrigger->SetActorLocation(exitRoom->GetStartPosition() + TriggerBoxSpawnOffset);
	_victoryTrigger->SetActorScale3D(TriggerBoxSpawnScale);

	_playerTopDownController->SetDefaultProperties(_playerCharacter, this);
	_playerTopDownController->SetCurrentRoom(spawnRoom);
	_playerTopDownController->SetPlayerRowAndColumn(row, column);
	_playerTopDownController->DisablePlayerTurn();
	
	_gameController->SetPlayerTopDownController(_playerTopDownController);
	_gameController->SetCurrentRoom(spawnRoom);

	_dungeonGen->SetPlayerController(_playerTopDownController);
	spawnRoom->SpawnEnemies();

	OnPlayerSpawnComplete.Broadcast(_playerCharacter);
}