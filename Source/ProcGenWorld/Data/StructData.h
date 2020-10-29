// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumData.h"
#include "StructData.generated.h"

USTRUCT(BlueprintType)
struct FUpgradeData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UpgradeType Upgrade;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString UpgradeName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString UpgradeDescription;
};