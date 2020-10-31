// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

/**
 *
 */
UCLASS()
class PROCGENWORLD_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = WinStatus, BlueprintReadWrite, EditAnywhere)
		bool DidPlayerWin;
};