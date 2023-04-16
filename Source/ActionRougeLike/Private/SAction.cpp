// Fill out your copyright notice in the Description page of Project Settings.
#include "SAction.h"
#include "SActionComponent.h"


void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));

	USActionComponent* Comp = GetOwningComponent();

	//add them all
	Comp->ActiveGameplayTags.AppendTags(GrantsTags);

	bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopping: %s"), *GetNameSafe(this));

	//check if boolean even on as santity
	ensureAlways(bIsRunning);

	USActionComponent* Comp = GetOwningComponent();

	//remove them all
	Comp->ActiveGameplayTags.RemoveTags(GrantsTags);

	bIsRunning = false;
}

bool USAction::CanStart_Implementation(AActor* Instigator)
{
	//to avoid running this when we have already started action
	if (IsRunning())
	{
		return false;
	}

	USActionComponent* Comp = GetOwningComponent();
	if (Comp->ActiveGameplayTags.HasAny(BlockedTags))
	{
		//if it contains any, return false
		return false;
	}
	else //we are allowed to do this action
		return true;

	
}

UWorld* USAction::GetWorld() const
{
	//this outer is set when creating action via newobject<>
	UActorComponent* Comp = Cast<UActorComponent>(GetOuter());
	if (Comp)
	{
		return Comp->GetWorld();
	}
	return nullptr;
}


USActionComponent* USAction::GetOwningComponent() const
{
	return Cast<USActionComponent>(GetOuter());
}


bool USAction::IsRunning() const
{
	return bIsRunning;
}