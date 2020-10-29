// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Data/EnumData.h"
#include "../Data/StructData.h"
#include "UpgradeController.generated.h"

class APlayerTopDownController;

UCLASS()
class PROCGENWORLD_API AUpgradeController : public AActor
{
	GENERATED_BODY()

	static const int MAX_UPGRADES = 3;
	APlayerTopDownController* _playerController;

public:
#pragma region Properties

	UPROPERTY(Category = Upgrade, EditAnywhere)
		TMap<UpgradeType, FUpgradeData> UpgradesData;

#pragma endregion

	AUpgradeController();
	virtual void Tick(float DeltaTime) override;

	void SetPlayerController(APlayerTopDownController* playerController);

	void ShowRandomUpgrades();

	UFUNCTION(Category = Upgrades, BlueprintImplementableEvent)
		void DisplayUpgradesUI(const TMap<UpgradeType, FUpgradeData> &upgrades);

	UFUNCTION(Category = Upgrades, BlueprintImplementableEvent)
		void ClearUpgradesUI();

	UFUNCTION(Category = Upgrade, BlueprintCallable)
		void HandleUpgradeClicked(UpgradeType upgradeType);

protected:
	virtual void BeginPlay() override;
};
