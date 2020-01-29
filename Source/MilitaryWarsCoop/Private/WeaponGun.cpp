// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/WeaponGun.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "../Public/WeaponGun.h"
#include "Engine.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "MilitaryWarsCoop.h"
#include "TimerManager.h"
#include "WeaponGun.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines For Weapons"),
	ECVF_Cheat);

// Sets default values
AWeaponGun::AWeaponGun()
{

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	BaseDamage = 20.0f;

	RateOfFire = 600;

	loadedAmmo = 30;
	ammoPool = 30;
}


void AWeaponGun::Fire()
{

	AActor* MyOwner = GetOwner();
	if (MyOwner) 
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FVector TracerEndPoint = TraceEnd;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			AActor* HitActor = Hit.GetActor();

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());


			float ActualDamage = BaseDamage;
			if(SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.0f;
			}


			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
		
			UParticleSystem* SelectedEffect = nullptr;
		    switch (SurfaceType)
			{
			case SURFACE_FLESHDEFAULT:
			case SURFACE_FLESHVULNERABLE:
				SelectedEffect = FleshImpactEffect;
				break;
			  default:
				  SelectedEffect = DefaultImpactEffect;
				  break;
			}

			if (SelectedEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			TracerEndPoint = Hit.ImpactPoint;
		}

		if(DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}

		PlayFireEffects(TracerEndPoint);
	}
	
	LastFireTime = GetWorld()->TimeSeconds;

}



void AWeaponGun::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimeHandle_TimeBetweenShots, this, &AWeaponGun::Fire, TimeBetweenShots, true, FirstDelay);

}

void AWeaponGun::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimeHandle_TimeBetweenShots);
}




void AWeaponGun::BeginPlay()
{
	Super::BeginPlay();
	TimeBetweenShots = 60 / RateOfFire;
}

void AWeaponGun::PlayFireEffects(FVector TraceEnd)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter("TracerTargetName", TraceEnd);
		}
	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
	    if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}
}



