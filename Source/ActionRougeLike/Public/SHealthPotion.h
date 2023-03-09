// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPickupBase.h"
#include "Components/StaticMeshComponent.h"
#include "SHealthPotion.generated.h"

UCLASS()
class ACTIONROUGELIKE_API ASHealthPotion : public ASPickupBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASHealthPotion();

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	float HealthIncrease;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	void OnTriggerTimer() override;
};
