// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class APlayerModel;
class ATile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLandDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMoveDelegate, FVector, TargetLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerReachedPosition);

UCLASS()
class PROCGENWORLD_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

		UPROPERTY(Category = Positions, VisibleDefaultsOnly)
		class USceneComponent* GroundCheckPoint;

	APlayerModel* _playerModel;

	const float FloatTolerance = 0.001f;

	float _currentZRotation;
	float _targetZRotation;
	float _lerpAmount;

	float _moveZ;
	float _moveX;

	bool _isGrounded;

	float _currentInputLockDelay;

#pragma region Player Movement

	void MoveVertical(float inputValue);
	void MoveHorizontal(float inputValue);
	void TurnPlayer(float inputValue);
	void LookUpPlayer(float inputValue);

	void JumpPlayer();

	void CheckIsOnGround();
	void UpdatePlayerModel();

#pragma endregion

#pragma region Utility Functions

	float LerpAngleDeg(float fromDegrees, float toDegrees, float progress);
	float To360Angle(float angle);

#pragma endregion

public:
#pragma region Parameters

	UPROPERTY(Category = Movement, EditAnywhere)
		float TurnSpeed;

	UPROPERTY(Category = Movement, EditAnywhere)
		float LookUpSpeed;

	UPROPERTY(Category = Movement, BlueprintReadonly)
		bool IsOnGround;

	UPROPERTY(Category = Movement, EditAnywhere)
		float InputLockTime;

	UPROPERTY(Category = Movement, BlueprintReadonly)
		USkeletalMeshComponent* PlayerModel;

	UPROPERTY(Category = Movement, EditAnywhere)
		float GroundCheckDistance;

	UPROPERTY(Category = Model, EditAnywhere)
		float ModelLerpSpeed;

	UPROPERTY(Category = Model, EditAnywhere)
		FVector ModelPositionOffset;

	UPROPERTY(Category = Model, EditAnywhere)
		float ModelRotationOffset;

	UPROPERTY(Category = Delegates, BlueprintAssignable)
		FJumpDelegate OnPlayerJumped;

	UPROPERTY(Category = Delegates, BlueprintAssignable)
		FLandDelegate OnPlayerLanded;

	UPROPERTY(Category = Delegates, BlueprintAssignable)
		FMoveDelegate OnPlayerMoved;

	UPROPERTY(Category = Delegates, BlueprintAssignable)
		FPlayerReachedPosition OnPlayerReachedPosition;

#pragma endregion

#pragma region External Movement

	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();

	bool MoveToTilePosition(FHitResult hitResult, ATile* tile);
	UFUNCTION(Category = AI, BlueprintCallable)
		void HandlePlayerReachedPosition();

#pragma endregion

	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
};
