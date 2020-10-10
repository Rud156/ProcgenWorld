// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerModel.h"

// Sets default values
APlayerModel::APlayerModel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerMesh"));
	PlayerMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APlayerModel::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerModel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlayerMeshComponent = PlayerMesh;
}

