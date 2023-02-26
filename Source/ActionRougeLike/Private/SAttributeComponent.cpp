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

bool USAttributeComponent::ApplyHealthChange(float Delta)
{
	//Health += Delta;
	UE_LOG(LogTemp, Log, TEXT("Health: %f"), Health);
	Health = FMath::Clamp(Health - Delta, 0.0f, HealthMax);
	UE_LOG(LogTemp, Log, TEXT("New Health: %f"), Health);
	OnHealthChanged.Broadcast(nullptr, this, Health, Delta);
	//for now it will return true, we can add to this later
	return true;
}
