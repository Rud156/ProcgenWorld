// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class PROCGENWORLD_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(Category = Camera, VisibleDefaultsOnly)
		class USpringArmComponent* CameraSpringArm;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly)
		class UCameraComponent* PlayerCamera;

#pragma region Player Movement

	void MoveVertical(float inputValue);
	void MoveHorizontal(float inputValue);
	void TurnPlayer(float inputValue);
	void LookUpPlayer(float inputValue);

	void JumpPlayer();
	void CrouchPlayer();

#pragma endregion

public:
#pragma region Parameters

	UPROPERTY(Category = Movement, EditAnywhere)
		float TurnSpeed;

	UPROPERTY(Category = Movement, EditAnywhere)
		float LookUpSpeed;

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
