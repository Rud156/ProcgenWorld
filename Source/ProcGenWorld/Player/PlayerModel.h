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
	APlayerModel();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Category = Mesh, BlueprintReadonly)
		USkeletalMeshComponent* PlayerMeshComponent;

protected:
	virtual void BeginPlay() override;
};
