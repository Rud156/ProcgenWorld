// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ProcGenWorld/Data/EnumData.h"
#include "PlayerTopDownController.generated.h"

class APlayerCharacter;
class AUpgradeController;
class ARoomGenerator;
class APlayerSpawn;
class AGameController;
class ATile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDied);

UCLASS()
class PROCGENWORLD_API APlayerTopDownController : public APawn
{
	GENERATED_BODY()

	UPROPERTY(Category = Actor, VisibleDefaultsOnly)
		class USceneComponent* TopDownSceneComponent;

	AGameController* _gameController;
	AUpgradeController* _upgradeController;
	
	APlayerCharacter* _playerCharacter;
	ARoomGenerator* _currentRoom;
	ATile* _lastClickedTile;

	bool _playerHasSpear;
	ActionType _lastPlayerAction;

	int _maxHP;
	int _currentHP;
	int _maxMana;
	int _currentMana;

	int _minSpearRadius;
	int _maxSpearRadius;

	int _minJumpRadius;
	int _maxJumpRadius;

	int _playerRoomRow;
	int _playerRoomColumn;

	bool _hasFreeMovement;
	bool _isPlayerTurn;

	void HandleMouseClicked();
	void Handle1Pressed();
	void Handle2Pressed();

	void ExecuteMoveToTileAction(FHitResult hitResult, ATile* tile);
	void ExecuteAttackTileAction(ATile* tile);
	void ExecutePushAction(ATile* tile);
	void ExecuteSpearThrowAction(ATile* tile);
	void ExecuteDashAction(FHitResult hitResult, ATile* tile);

protected:
	virtual void BeginPlay() override;

public:

#pragma region Properties

	UPROPERTY(Category = Position, EditAnywhere)
		FVector FollowOffset;

	UPROPERTY(Category = Display, EditAnywhere)
		TSubclassOf<class AUpgradeController> UpgradeController;

	UPROPERTY(Category = PlayerStats, EditAnywhere)
		float MaxHealth;

	UPROPERTY(Category = PlayerStats, EditAnywhere)
		float MaxMana;

	UPROPERTY(Category = PlayerStats, EditAnywhere)
		float MinSpearRadius;

	UPROPERTY(Category = PlayerStats, EditAnywhere)
		float MaxSpearRadius;

	UPROPERTY(Category = PlayerStats, EditAnywhere)
		float MinDashRadius;

	UPROPERTY(Category = PlayerStats, EditAnywhere)
		float MaxDashRadius;

	UPROPERTY(Category = PlayerStats, EditAnywhere)
		int DashManaCost;

	UPROPERTY(Category = PlayerStats, EditAnywhere)
		float DefaultMovementSpeed;

	UPROPERTY(Category = PlayerStats, EditAnywhere)
		float DashMovementSpeed;

	UPROPERTY(Category = PlayerStats, EditAnywhere)
		float MeleeDamageAmount;

	UPROPERTY(Category = PlayerStats, EditAnywhere)
		float EnemyPushManaCost;

	UPROPERTY(Category = PlayerStats, EditAnywhere)
		float EnemyPushDamage;

	UPROPERTY(Category = Spawning, BlueprintAssignable)
		FPlayerDied OnPlayerDied;

#pragma endregion

	APlayerTopDownController();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetDefaultProperties(APlayerCharacter* playerCharacter, AGameController* gameController);
	void SetCurrentRoom(ARoomGenerator* roomGenerator);
	void SetPlayerRowAndColumn(int row, int column);

	void EnablePlayerTurn();
	void DisablePlayerTurn();
	UFUNCTION(Category = Display, BlueprintCallable, BlueprintPure)
		bool GetIsPlayerTurn();

	void EnableFreeMovement();
	void DisableFreeMovement();

	ARoomGenerator* GetRoomInstance();
	int GetPlayerRow();
	int GetPlayerColumn();
	UFUNCTION()
		void HandlePlayerReachedPosition();

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

	void ApplyUpgrade(UpgradeType upgradeType);

	UFUNCTION(Category = Actions, BlueprintCallable)
		void HandlePlayerMoveAction();

	UFUNCTION(Category = Actions, BlueprintCallable)
		void HandlePlayerPushAction();

	UFUNCTION(Category = Actions, BlueprintCallable)
		void HandlePlayerSpearAction();

	UFUNCTION(Category = Actions, BlueprintCallable)
		void HandlePlayerAttackAction();

	UFUNCTION(Category = Actions, BlueprintCallable)
		void HandlePlayerDashAction();
};
