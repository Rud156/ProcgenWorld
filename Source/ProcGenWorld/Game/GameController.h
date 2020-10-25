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

	bool _isPlayerTurn;
	float _currentTurnTime;

#pragma endregion

	void BeginPlayerTurn();

public:
	// Sets default values for this actor's properties
	AGameController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Category = Timing, EditAnywhere)
		float DefaultTurnTime;

	UFUNCTION(Category = Timing, BlueprintCallable)
		void EndPlayerTurn();

	UFUNCTION(Category = Timing, BlueprintCallable, BlueprintPure)
		bool GetPlayerTurnStatus();

	UFUNCTION(Category = Timing, BlueprintCallable, BlueprintPure)
		float GetTurnTime();

	void SetPlayerTopDownController(APlayerTopDownController* playerTopDownController);
	void SetCurrentRoom(ARoomGenerator* currentRoom);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
