// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSpawn.h"
#include "PlayerModel.h"
#include "PlayerCharacter.h"
#include "../Room/DungeonGenerator.h"
#include "../Room/RoomGenerator.h"

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
		_dungeonGen->OnGenerationComplete.AddDynamic(this, &APlayerSpawn::RoomGeneratonComplete);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Unable to find Dungeon Generator!!!");
	}
}

// Called every frame
void APlayerSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerSpawn::RoomGeneratonComplete()
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

	int spawnRow = _dungeonGen->GetSpawnRow();
	int spawnColumn = _dungeonGen->GetSpawnColumn();
	ARoomGenerator* spawnRoom = _dungeonGen->GetRoom(spawnRow, spawnColumn);

	FVector startPosition = spawnRoom->GetStartPosition();
	startPosition += SpawnOffset;

	AActor* playerModelActor = GetWorld()->SpawnActor(PlayerModelPrefab, &startPosition, &FRotator::ZeroRotator);
	AActor* playerCharacterActor = GetWorld()->SpawnActor(PlayerCharacterPrefab, &startPosition, &FRotator::ZeroRotator);

	_playerModel = Cast<APlayerModel>(playerModelActor);
	_playerCharacter = Cast<APlayerCharacter>(playerCharacterActor);

	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	controller->UnPossess();
	controller->Possess(_playerCharacter);

	int exitRow = _dungeonGen->GetExitRow();
	int exitColumn = _dungeonGen->GetExitColumn();
	ARoomGenerator* exitRoom = _dungeonGen->GetRoom(exitRow, exitColumn);

	_victoryTrigger->SetActorLocation(exitRoom->GetStartPosition() + TriggerBoxSpawnOffset);
	_victoryTrigger->SetActorScale3D(TriggerBoxSpawnScale);
}