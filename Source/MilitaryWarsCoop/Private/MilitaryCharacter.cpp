// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/MilitaryCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "MilitaryWarsCoop.h"
#include "Engine.h"
#include "WeaponGun.h"


// Sets default values
AMilitaryCharacter::AMilitaryCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20;

	WeaponAttachSocketName = "WeaponSocket";

	loadedAmmo = 30;
	ammoPool = 30;
}

// Called when the game starts or when spawned
void AMilitaryCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = CameraComp->FieldOfView;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentWeapon = GetWorld()->SpawnActor<AWeaponGun>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if(CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	}
}

void AMilitaryCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void AMilitaryCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void AMilitaryCharacter::BeginCrouch()
{
	Crouch();
}

void AMilitaryCharacter::EndCrouch()
{
	UnCrouch();
}


void AMilitaryCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void AMilitaryCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void AMilitaryCharacter::StartFire()
{

	if(CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}

}

void AMilitaryCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}



// Called every frame
void AMilitaryCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);

}

// Called to bind functionality to input
void AMilitaryCharacter::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMilitaryCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMilitaryCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &AMilitaryCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &AMilitaryCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMilitaryCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMilitaryCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AMilitaryCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AMilitaryCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMilitaryCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMilitaryCharacter::StopFire);

}

FVector AMilitaryCharacter::GetPawnViewLocation() const
{
	if(CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

