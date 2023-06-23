// Fill out your copyright notice in the Description page of Project Settings.
#include "SActionComponent.h"
#include "SAction.h"
#include <ActionRougeLike/ActionRougeLike.h>
#include "Engine/ActorChannel.h"
#include <Runtime/Engine/Public/Net/UnrealNetwork.h>


// Sets default values for this component's properties
USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void USActionComponent::BeginPlay()
{
	Super::BeginPlay();


	//make sure this only runs on server (server only)
	if (GetOwner()->HasAuthority())
	{
		for (TSubclassOf<USAction> ActionClass : DefaultActions)
		{
			AddAction(GetOwner(), ActionClass);
		}
	}


}

// Called every frame
void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//debug for helping illustrate tags
	//to string simple has less markup and more efficient
	//FString DebugMessage = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMessage);

	//draw all actions
	for (USAction* Action : Actions)
	{
		FColor TextColor = Action->IsRunning() ? FColor::Blue : FColor::White;
		FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s : IsRunning: %s : Outer: %s"),
			*GetNameSafe(GetOwner()),
			*Action->ActionName.ToString(),
			Action->IsRunning() ? TEXT("True") : TEXT("False"),
			*GetNameSafe(Action->GetOuter()));
		LogOnScreen(this, ActionMsg, TextColor, 0.0f);
	}
}



void USActionComponent::AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}

	//instantiate an object
	//outer, whoever owns this object in a way, pass in this because the action component will own this
	//next is action class
	USAction* NewAction = NewObject<USAction>(GetOwner(), ActionClass);
	//add the action to our array
	if (ensure(NewAction))
	{
		NewAction->Initialize(this);
		Actions.Add(NewAction);

		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
			NewAction->StartAction(Instigator);
	}
}

bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (!Action->CanStart(Instigator))
			{
				FString FailedMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailedMsg);

				//could come back to this if another action needs to be checked and run instead
				continue;
			}

			//blackholes cost rage

			//to avoid an infi loop
			if (!GetOwner()->HasAuthority()) {
				ServerStartAction(Instigator, ActionName);
			}
			

			Action->StartAction(Instigator);
			return true;
		}
	}
	return false;
}

bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (Action->IsRunning()) {
				Action->StopAction(Instigator);
				return true;
			}
		}
	}
	return false;
}

void USActionComponent::RemoveAction(USAction* ActionToRemove)
{
	if (!ensure(ActionToRemove && !ActionToRemove->IsRunning()))
	{
		return;
	}
	Actions.Remove(ActionToRemove);
}

bool USActionComponent::CheckAction(TSubclassOf<USAction> ActionClass)
{
	return DefaultActions.Contains(ActionClass);
	//or
	//for (USAction* Action : Actions)
	//{
		//if (Action && Action->IsA(ActionClass))
		//{
			//return ActionClass;
		//}
	//}
	//return nullptr;
}

bool USActionComponent::CheckActionName(FName ActionName)
{
	for (USAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			return true;
		}
	}

	return false;
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);

}

//we shouldnt need to add the header file since we marked a var as replciated. Its only generated h should include it
void  USActionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USActionComponent, Actions);
}


bool USActionComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (USAction* Action : Actions)
	{
		if (Action)
		{
			//using bitwise to write to WroteSomthing
			//make sure that it keeps track of wheather or not it wrote somehting
			WroteSomething |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);

		}
	}

	return WroteSomething;
}