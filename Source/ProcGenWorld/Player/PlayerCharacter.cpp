// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

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
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &APlayerCharacter::CrouchPlayer);
}

#pragma region Player Movement

void APlayerCharacter::MoveVertical(float inputValue)
{
	AddMovementInput(GetActorForwardVector(), inputValue);
}

void APlayerCharacter::MoveHorizontal(float inputValue)
{
	AddMovementInput(GetActorRightVector(), inputValue);
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

void APlayerCharacter::CrouchPlayer()
{
}

#pragma endregion