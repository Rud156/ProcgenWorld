// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomGenerator.generated.h"

UCLASS()
class PROCGENWORLD_API ARoomGenerator : public AActor
{
	GENERATED_BODY()

private:
	int _rowCount;
	int _columnCount;

	FVector _startPosition;

	FVector _topRowDoorPosition;
	FVector _bottomRowDoorPosition;
	FVector _leftColumnDoorPosition;
	FVector _rightColumnDoorPosition;

	FString _roomName;
	TArray<AActor*> _walls;

	FVector _lerpStartPositon;
	FVector _lerpTargetPosition;
	float _lerpAmount;
	bool _isLerpActive;

	void RenderRoomFromString(FString roomString, FVector startPosition);

public:
#pragma region Parameters

	UPROPERTY(Category = Wall, EditAnywhere)
		TSubclassOf<class AActor> WallPrefab;

	UPROPERTY(Category = Wall, EditAnywhere)
		TSubclassOf<class AActor> TestDoorPrefab;

	UPROPERTY(Category = Wall, EditAnywhere)
		float WallWidth;

	UPROPERTY(Category = Wall, EditAnywhere)
		float WallThickness;

	UPROPERTY(Category = WallMovement, EditAnywhere)
		float LerpSpeed;

#pragma endregion

	// Sets default values for this actor's properties
	ARoomGenerator();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void LoadRoomFromFile(FString roomName, FVector startPosition);

	FString GetRoomName();
	FVector GetStartPosition();
	void UpdateRoomPosition(FVector offset);

	FVector GetTopRowDoorPosition();
	FVector GetBottomRowDoorPosition();
	FVector GetLeftColumnDoorPosition();
	FVector GetRightColumnDoorPosition();

	int GetRowCount();
	int GetColumnCount();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Destroyed() override;
};
