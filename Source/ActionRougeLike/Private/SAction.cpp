// Fill out your copyright notice in the Description page of Project Settings.
#include "SAction.h"
#include "SActionComponent.h"


void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));

	USActionComponent* Comp = GetOwningComponent();

	//add them all
	Comp->ActiveGameplayTags.AppendTags(GrantsTags);
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopping: %s"), *GetNameSafe(this));

	USActionComponent* Comp = GetOwningComponent();

	//remove them all
	Comp->ActiveGameplayTags.RemoveTags(GrantsTags);
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