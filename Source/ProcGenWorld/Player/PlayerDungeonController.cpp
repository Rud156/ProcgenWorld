// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDungeonController.h"
#include "PlayerSpawn.h"
#include "PlayerCharacter.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerDungeonController::APlayerDungeonController()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlayerDungeonController::BeginPlay()
{
	Super::BeginPlay();

	AActor* defaultPawn = UGameplayStatics::GetActorOfClass(GetWorld(), AActor::StaticClass());
	if (defaultPawn != nullptr) {
		APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		playerController->UnPossess();
		playerController->Possess(this);
	}

	auto playerSpawnActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerSpawn::StaticClass());
	_playerSpawn = Cast<APlayerSpawn>(playerSpawnActor);
	if (_playerSpawn != nullptr) {
		_playerSpawn->OnPlayerSpawnComplete.AddDynamic(this, &APlayerDungeonController::HandlePlayerSpawnComplete);
	}
}

// Called every frame
void APlayerDungeonController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerDungeonController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Up", EInputEvent::IE_Pressed, this, &APlayerDungeonController::MoveUp);
	PlayerInputComponent->BindAction("Down", EInputEvent::IE_Pressed, this, &APlayerDungeonController::MoveDown);
	PlayerInputComponent->BindAction("Left", EInputEvent::IE_Pressed, this, &APlayerDungeonController::MoveLeft);
	PlayerInputComponent->BindAction("Right", EInputEvent::IE_Pressed, this, &APlayerDungeonController::MoveRight);
}

void APlayerDungeonController::MoveUp()
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Up Pressed");
}

void APlayerDungeonController::MoveDown()
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Down Pressed");
}

void APlayerDungeonController::MoveLeft()
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Left Pressed");
}

void APlayerDungeonController::MoveRight()
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Right Pressed");
}

void APlayerDungeonController::HandlePlayerSpawnComplete(APlayerCharacter* PlayerCharacter)
{
	_playerCharacter = PlayerCharacter;
}
