// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "PlayerModel.h"
#include "AIPlayerController.h"
#include "../Room/Tile.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GroundCheckPoint = CreateDefaultSubobject<USceneComponent>(TEXT("GroundCheckPoint"));
	GroundCheckPoint->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	auto modelActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerModel::StaticClass());
	_playerModel = Cast<APlayerModel>(modelActor);

	float rotationAngle = ModelRotationOffset + GetActorRotation().Yaw;
	_playerModel->SetActorRotation(FRotator(0, rotationAngle, 0));
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePlayerModel();
	CheckIsOnGround();

	_playerModel->SetActorLocation(GetActorLocation() + ModelPositionOffset);
	PlayerModel = _playerModel->PlayerMeshComponent;

	if (_currentInputLockDelay > 0) {
		_currentInputLockDelay -= DeltaTime;
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//check(PlayerInputComponent);

	//PlayerInputComponent->BindAxis("Vertical", this, &APlayerCharacter::MoveVertical);
	//PlayerInputComponent->BindAxis("Horizontal", this, &APlayerCharacter::MoveHorizontal);
	//PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::TurnPlayer);
	//PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUpPlayer);

	//PlayerInputComponent->BindAction("Up", EInputEvent::IE_Pressed, this, &APlayerCharacter::MoveUp);
	//PlayerInputComponent->BindAction("Down", EInputEvent::IE_Pressed, this, &APlayerCharacter::MoveDown);
	//PlayerInputComponent->BindAction("Left", EInputEvent::IE_Pressed, this, &APlayerCharacter::MoveLeft);
	//PlayerInputComponent->BindAction("Right", EInputEvent::IE_Pressed, this, &APlayerCharacter::MoveRight);
	//PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerCharacter::JumpPlayer);
	//PlayerInputComponent->BindAction("LeftClick", EInputEvent::IE_Pressed, this, &APlayerCharacter::HandleMouseClicked);
}

#pragma region Player Movement

void APlayerCharacter::MoveVertical(float inputValue)
{
	AddMovementInput(GetActorForwardVector(), inputValue);
	_moveZ = inputValue;
}

void APlayerCharacter::MoveHorizontal(float inputValue)
{
	AddMovementInput(GetActorRightVector(), inputValue);
	_moveX = inputValue;
}

void APlayerCharacter::TurnPlayer(float inputValue)
{
	AddControllerYawInput(TurnSpeed * inputValue * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpPlayer(float inputValue)
{
	AddControllerPitchInput(LookUpSpeed * -inputValue * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::JumpPlayer()
{
	OnPlayerJumped.Broadcast();
	Jump();
}

void APlayerCharacter::CheckIsOnGround()
{
	FVector startPosition = GroundCheckPoint->GetComponentLocation();
	FVector endPosition = startPosition + FVector::DownVector * GroundCheckDistance;

	FHitResult result;
	FCollisionQueryParams collisionParams;

	IsOnGround = GetWorld()->LineTraceSingleByChannel(result, startPosition, endPosition, ECollisionChannel::ECC_Visibility, collisionParams);
	if (IsOnGround) {
		if (!_isGrounded) {
			OnPlayerLanded.Broadcast();
		}

		_isGrounded = true;
	}
	else {
		_isGrounded = false;
	}
}

void APlayerCharacter::UpdatePlayerModel()
{
	/*if (FMath::Abs(_moveX) < FloatTolerance && FMath::Abs(_moveZ) < FloatTolerance) {
		return;
	}

	float angle = FMath::Atan2(_moveX, _moveZ);
	float angleDegree = To360Angle(FMath::RadiansToDegrees(angle) + ModelRotationOffset + GetActorRotation().Yaw);

	if (_targetZRotation != angleDegree) {
		_targetZRotation = angleDegree;
		_currentZRotation = To360Angle(_playerModel->GetActorRotation().Yaw);
		_lerpAmount = 0;
	}

	_lerpAmount += ModelLerpSpeed * GetWorld()->GetDeltaSeconds();
	if (_lerpAmount < 1) {
		float mappedAngle = LerpAngleDeg(_currentZRotation, _targetZRotation, _lerpAmount);
		auto targetRotation = FRotator(0, mappedAngle, 0);

		_playerModel->SetActorRotation(targetRotation);
	}*/


	float angleDegree = To360Angle(ModelRotationOffset + GetActorRotation().Yaw);
	_playerModel->SetActorRotation(FRotator(0, angleDegree, 0));;
}

float APlayerCharacter::LerpAngleDeg(float fromDegrees, float toDegrees, float progress)
{
	float delta = FMath::Fmod((toDegrees - fromDegrees + 360 + 180), 360) - 180.0f;
	return FMath::Fmod(fromDegrees + delta * progress + 360, 360);
}

float APlayerCharacter::To360Angle(float angle)
{
	while (angle < 0.0f)
		angle += 360.0f;
	while (angle >= 360.0f)
		angle -= 360.0f;

	return angle;
}

#pragma endregion

#pragma region External Movement

void APlayerCharacter::MoveUp()
{
	_currentInputLockDelay = InputLockTime;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Move Up");
}

void APlayerCharacter::MoveDown()
{
	_currentInputLockDelay = InputLockTime;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Move Down");
}

void APlayerCharacter::MoveLeft()
{
	_currentInputLockDelay = InputLockTime;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Move Left");
}

void APlayerCharacter::MoveRight()
{
	_currentInputLockDelay = InputLockTime;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Move Right");
}

void APlayerCharacter::HandleMouseClicked(FHitResult hitResult, ATile* tile)
{
	if (_currentInputLockDelay > 0) {
		return;
	}
	if (!tile->IsTileMarked()) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Tile is UnMarked To Move");
		return;
	}

	_currentInputLockDelay = InputLockTime;

	FVector targetLocation = tile->TileCenter;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, *targetLocation.ToString());

	AAIPlayerController* aiController = Cast<AAIPlayerController>(GetController());
	if (aiController != nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "AI Controller Exists");
		OnPlayerMoved.Broadcast(targetLocation);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "AI Controller Does Not Exists");
	}
}

#pragma endregion