// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class APlayerModel;

UCLASS()
class PROCGENWORLD_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(Category = Camera, VisibleDefaultsOnly)
		class USpringArmComponent* CameraSpringArm;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly)
		class UCameraComponent* PlayerCamera;

	APlayerModel* _playerModel;

	const float FloatTolerance = 0.001f;

	float _currentZRotation;
	float _targetZRotation;
	float _lerpAmount;

	float _moveZ;
	float _moveX;

#pragma region Player Movement

	void MoveVertical(float inputValue);
	void MoveHorizontal(float inputValue);
	void TurnPlayer(float inputValue);
	void LookUpPlayer(float inputValue);

	void JumpPlayer();
	void UpdatePlayerModel();

#pragma endregion

	float LerpAngleDeg(float fromDegrees, float toDegrees, float progress);
	float To360Angle(float angle);

public:
#pragma region Parameters

	UPROPERTY(Category = Movement, EditAnywhere)
		float TurnSpeed;

	UPROPERTY(Category = Movement, EditAnywhere)
		float LookUpSpeed;

	UPROPERTY(Category = Model, EditAnywhere)
		float ModelLerpSpeed;

	UPROPERTY(Category = Model, EditAnywhere)
		FVector ModelPositionOffset;

	UPROPERTY(Category = Model, EditAnywhere)
		float ModelRotationOffset;

#pragma endregion

	// Sets default values for this character's properties
	APlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
