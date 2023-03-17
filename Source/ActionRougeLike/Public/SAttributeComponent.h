// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

class USAttributeComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthCHanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewHealth, float, Delta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROUGELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USAttributeComponent();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float Health;

	//other examples
	//HealthMax, stamina, strength
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float HealthMax = 100;

public:

	

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;

	UPROPERTY(BlueprintAssignable)
	FOnHealthCHanged OnHealthChanged;

	//return if the change succeeded
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(AActor* Instigator, float Delta);

	UFUNCTION(BlueprintCallable)
	bool IsUnderMaxHealth() const;
		
};
