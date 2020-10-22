// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerTopDownController.h"
#include "PlayerCharacter.h"
#include "../Room/Tile.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

// Sets default values
APlayerTopDownController::APlayerTopDownController()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlayerTopDownController::BeginPlay()
{
	Super::BeginPlay();

	auto playerCharacterActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(playerCharacterActor);
	if (playerCharacter != nullptr) {
		_playerCharacter = playerCharacter;

		APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		_playerCharacter->SetPlayerCamera(playerController);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Player Character Not Found");
	}
}

// Called every frame
void APlayerTopDownController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector targetLocation = _playerCharacter->GetActorLocation() + FollowOffset;
	SetActorLocation(targetLocation);
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
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Mouse Clicked");

	FHitResult hitResult;

	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, hitResult);

	auto hitActor = hitResult.GetActor();
	ATile* tile = Cast<ATile>(hitActor);

	if (tile != nullptr) {
		_playerCharacter->HandleMouseClicked(hitResult, tile);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, *hitActor->GetName());
	}
}

