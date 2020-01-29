// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponGun.h"
#include "Engine.h"
#include "ProjectileWeaponGun.generated.h"

/**
 * 
 */
UCLASS()
class MILITARYWARSCOOP_API AProjectileWeaponGun : public AWeaponGun
{
	GENERATED_BODY()

protected:

	virtual void Fire();

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileWeaponGun")
	TSubclassOf<AActor> ProjectileClass;
 

};
