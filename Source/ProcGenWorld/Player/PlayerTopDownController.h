// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerTopDownController.generated.h"

class APlayerCharacter;

UCLASS()
class PROCGENWORLD_API APlayerTopDownController : public APawn
{
	GENERATED_BODY()

	APlayerCharacter* _playerCharacter;

	UFUNCTION()
		void DelayedSwitchCamera();

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
};
