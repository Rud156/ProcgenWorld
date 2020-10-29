// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameController.generated.h"

class APlayerTopDownController;
class ARoomGenerator;


UCLASS()
class PROCGENWORLD_API AGameController : public AActor
{
	GENERATED_BODY()

#pragma region Properties

	APlayerTopDownController* _playerTopDownController;
	ARoomGenerator* _currentRoom;

	bool _gameTurnActive;
	bool _isPlayerTurn;

	bool _nextTurnPlayer;
	float _currentTurnEndDelay;

	float _currentTurnTime;

#pragma endregion

	void BeginPlayerTurn();
	void ExecuteEnemyAI();

public:
	// Sets default values for this actor's properties
	AGameController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Category = Timing, EditAnywhere)
		float PlayerTurnWaitTime;

	UPROPERTY(Category = Timing, EditAnywhere)
		float AITurnWaitTime;

	UPROPERTY(Category = Timing, EditAnywhere)
		float TurnEndDelay;

	UFUNCTION(Category = Timing, BlueprintCallable)
		void EndPlayerTurn();

	UFUNCTION(Category = Timing, BlueprintCallable, BlueprintPure)
		bool GetPlayerTurnStatus();

	UFUNCTION(Category = Timing, BlueprintCallable, BlueprintPure)
		float GetTurnTime();

	UFUNCTION(Category = Timing, BlueprintCallable, BlueprintPure)
		float GetTurnDelayTime();

	void SetPlayerTopDownController(APlayerTopDownController* playerTopDownController);
	void SetCurrentRoom(ARoomGenerator* currentRoom);
	UFUNCTION()
		void HandleRoomCleared();

	void ResetPlayerTurnTime();

	void BeginGameTurn();
	void EndGameTurn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
