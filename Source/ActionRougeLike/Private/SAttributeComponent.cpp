// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"
#include "Math/UnrealMathUtility.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	
	
	// set to max health on start
	Health = HealthMax;
	UE_LOG(LogTemp, Log, TEXT("Starting Health: %f"), Health);
	UE_LOG(LogTemp, Log, TEXT("Max Health: %f"), HealthMax);
	
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::ApplyHealthChange(AActor* Instigator, float Delta)
{
	float OldHealth = Health;


	//Health += Delta;
	//UE_LOG(LogTemp, Log, TEXT("Health: %f"), Health);
	Health = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	float HealthDelta = Health - OldHealth;
	OnHealthChanged.Broadcast(Instigator, this, Health, HealthDelta);

	//UE_LOG(LogTemp, Log, TEXT("New Health: %f"), Health);
	//OnHealthChanged.Broadcast(nullptr, this, Health, Delta);
	//for now it will return true, we can add to this later

	//make sure we know if this even applied a health change
	return HealthDelta != 0;
}

bool USAttributeComponent::IsUnderMaxHealth() const
{
	return Health < HealthMax;
}
