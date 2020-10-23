// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UCLASS()
class PROCGENWORLD_API ATile : public AActor
{
	GENERATED_BODY()

		UPROPERTY(Category = Mesh, VisibleDefaultsOnly)
		USceneComponent* TileParent;

public:
#pragma region Properties

	UPROPERTY(BlueprintReadOnly)
		FVector TileCenter;

#pragma endregion

	// Sets default values for this actor's properties
	ATile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
