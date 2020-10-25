// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ActionType : uint8
{
	Move,
	Attack,
	Push,
	SpearThrow
};

UENUM(BlueprintType)
enum class UpgradeType : uint8
{
	RestoreHP,
	IncreaseHP,
	IncreaseMana,
	IncreaseJump
};

UENUM(BlueprintType)
enum class TileType : uint8
{
	FloorTile,
	LavaTile,
	ExitTile,
	WallTile,
	VictoryTile,
	UpgradeTile,
	PickupTile
};

UENUM(BlueprintType)
enum class PickupType : uint8
{
	Spear
};

UENUM(BlueprintType)
enum class WorldElementType : uint8
{
	Player,
	Enemy,
	LavaTile,
	Floor,
};