// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGenerator.generated.h"

UCLASS()
class PROCGENWORLD_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()

public:
#pragma region Properties

	UPROPERTY(Category = Rooms, EditAnywhere)
		TArray<FString> RoomNames;

	UPROPERTY(Category = Rooms, EditAnywhere)
		TSubclassOf<class ARoomGenerator> RoomGenerator;

#pragma endregion

	// Sets default values for this actor's properties
	ADungeonGenerator();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
