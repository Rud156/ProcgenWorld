// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerTopDownController.generated.h"

class APlayerCharacter;
class ARoomGenerator;
class APlayerSpawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDied);

UCLASS()
class PROCGENWORLD_API APlayerTopDownController : public APawn
{
	GENERATED_BODY()

	UPROPERTY(Category = Actor, VisibleDefaultsOnly)
		class USceneComponent* TopDownSceneComponent;

	APlayerCharacter* _playerCharacter;
	APlayerSpawn* _playerSpawn;
	ARoomGenerator* _currentRoom;

	int _maxHP;
	int _currentHP;
	int _maxMana;
	int _currentMana;

	int _playerRoomRow;
	int _playerRoomColumn;

	bool _isPlayerTurn;

	void HandleMouseClicked();

protected:
	virtual void BeginPlay() override;

public:

#pragma region Properties

	UPROPERTY(Category = Position, EditAnywhere)
		FVector FollowOffset;

	UPROPERTY(Category = PlayerStats, EditAnywhere)
		float MaxHealth;

	UPROPERTY(Category = PlayerStats, EditAnywhere)
		float MaxMana;

	UPROPERTY(Category = Spawning, BlueprintAssignable)
		FPlayerDied OnPlayerDied;

#pragma endregion

	APlayerTopDownController();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetDefaultProperties(APlayerCharacter* playerCharacter, APlayerSpawn* playerSpawn);
	void SetCurrentRoom(ARoomGenerator* roomGenerator);
	void SetPlayerRowAndColumn(int row, int column);

	void EnablePlayerTurn();
	void DisablePlayerTurn();

	int GetPlayerRow();
	int GetPlayerColumn();

	UFUNCTION(Category = Display, BlueprintCallable, BlueprintPure)
		int GetPlayerHealth();
	UFUNCTION(Category = Display, BlueprintCallable, BlueprintPure)
		int GetPlayerMaxHealth();
	UFUNCTION(Category = Display, BlueprintCallable)
		void ResetPlayerHealth();
	void IncreasePlayerHealth(int amount);
	UFUNCTION(Category = Display, BlueprintCallable)
		void TakeDamage(int damageAmount);
	void HandlePlayerDied();

	UFUNCTION(Category = Display, BlueprintCallable, BlueprintPure)
		int GetPlayerMana();
	UFUNCTION(Category = Display, BlueprintCallable, BlueprintPure)
		int GetPlayerMaxMana();
	UFUNCTION(Category = Display, BlueprintCallable)
		void ResetPlayerMana();
	void IncreasePlayerMana(int amount);
	bool HasMana(int manaAmount);
	void UseMana(int manaAmount);
};
