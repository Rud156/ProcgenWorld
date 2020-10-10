// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerModel.generated.h"

UCLASS()
class PROCGENWORLD_API APlayerModel : public AActor
{
	GENERATED_BODY()

	UPROPERTY(Category = Mesh, VisibleDefaultsOnly)
	class USkeletalMeshComponent* PlayerMesh;

public:
	// Sets default values for this actor's properties
	APlayerModel();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
