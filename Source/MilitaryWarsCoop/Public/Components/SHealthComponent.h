// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"



UCLASS( ClassGroup=(MilitaryWars), meta=(BlueprintSpawnableComponent) )
class MILITARYWARSCOOP_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float DefaultHealth;

	void HandleTakeAnyDamage(AActor* DamagedActor, float  Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

};
