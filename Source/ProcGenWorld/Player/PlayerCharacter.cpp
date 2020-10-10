// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "PlayerModel.h"

#include "Kismet/GameplayStatics.h"

#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(CameraSpringArm);
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
	_playerModel->SetActorLocation(GetActorLocation() + ModelPositionOffset);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Vertical", this, &APlayerCharacter::MoveVertical);
	PlayerInputComponent->BindAxis("Horizontal", this, &APlayerCharacter::MoveHorizontal);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::TurnPlayer);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUpPlayer);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerCharacter::JumpPlayer);
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
	AddControllerPitchInput(LookUpSpeed * inputValue * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::JumpPlayer()
{
	Jump();
}

void APlayerCharacter::UpdatePlayerModel()
{
	if (FMath::Abs(_moveX) < FloatTolerance && FMath::Abs(_moveZ) < FloatTolerance) {
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
	}
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