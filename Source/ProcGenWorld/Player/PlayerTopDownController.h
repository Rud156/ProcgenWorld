// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerTopDownController.generated.h"

class APlayerCharacter;
class ARoomGenerator;
class APlayerSpawn;

UCLASS()
class PROCGENWORLD_API APlayerTopDownController : public APawn
{
	GENERATED_BODY()

		UPROPERTY(Category = Actor, VisibleDefaultsOnly)
		class USceneComponent* TopDownSceneComponent;

	APlayerCharacter* _playerCharacter;
	APlayerSpawn* _playerSpawn;
	ARoomGenerator* _currentRoom;

	int _playerRoomRow;
	int _playerRoomColumn;

	void HandleMouseClicked();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(Category = Position, EditAnywhere)
		FVector FollowOffset;

	// Sets default values for this pawn's properties
	APlayerTopDownController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetDefaultProperties(APlayerCharacter* playerCharacter, APlayerSpawn* playerSpawn);
	void SetCurrentRoom(ARoomGenerator* roomGenerator);
	void SetPlayerRowAndColumn(int row, int column);
};
