// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradeController.h"
#include "PlayerTopDownController.h"

// Sets default values
AUpgradeController::AUpgradeController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AUpgradeController::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AUpgradeController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUpgradeController::SetPlayerController(APlayerTopDownController* playerController)
{
	_playerController = playerController;
}

void AUpgradeController::EnableRandomUpgrades()
{
	TArray<UpgradeType> possibleUpgrades;
	UpgradesData.GetKeys(possibleUpgrades);

	possibleUpgrades.Sort([this](const UpgradeType Item1, const UpgradeType Item2)
		{
			return FMath::FRand() <= 0.5f;
		});

	TMap<UpgradeType, FString> upgrades = TMap<UpgradeType, FString>();
	for (int i = 0; i < MAX_UPGRADES; i++)
	{
		upgrades.Add(possibleUpgrades[0], UpgradesData[possibleUpgrades[0]]);
	}

	DisplayUpgradesUI(upgrades);
}

void AUpgradeController::HandleUpgradeClicked(UpgradeType upgradeType)
{
	_playerController->ApplyUpgrade(upgradeType);
	ClearUpgradesUI();
}
