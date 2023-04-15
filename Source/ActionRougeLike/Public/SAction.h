// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "SActionComponent.h"
#include "SAction.generated.h"

class UWorld;

UCLASS(Blueprintable)
class ACTIONROUGELIKE_API USAction : public UObject
{
	GENERATED_BODY()
	
protected:

	UFUNCTION(BlueprintCallable, Category = "Action")
	USActionComponent* GetOwningComponent() const;

	//tags added to owing actor when activated, removed when action stops
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer GrantsTags;

	//action can only start if owningactor has none of these tags applied
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer BlockedTags;

public:

	//Action nickname to start/stop without a reference to the object
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FName ActionName;

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* Instigator);
		
	UWorld* GetWorld() const override;

};
