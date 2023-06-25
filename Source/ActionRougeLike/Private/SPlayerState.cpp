// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include <ActionRougeLike/Public/SSaveGame.h>

ASPlayerState::ASPlayerState()
{
	Credits = 0;

}

int ASPlayerState::GetCredits()
{
	return Credits;
}

bool ASPlayerState::RemoveCredits(int32 delta)
{
	if (!ensure(delta > 0.0f))
		return false;
	
	//UE_LOG(LogTemp, Log, TEXT("in funct - current total: %d"), Credits);
	//UE_LOG(LogTemp, Log, TEXT("in funct - value to remove: %d"), value);
	if (delta <= Credits)
	{
		//int temp = Credits - value;
		//Credits = temp;
		Credits -= delta;
		OnCreditsChanged.Broadcast(this, Credits, -delta);
		//UE_LOG(LogTemp, Log, TEXT("in funct - final val: %d"), Credits);
		return true;
	}
	return false;
}

int ASPlayerState::AddCredits(int32 delta)
{

	if (!ensure(delta > 0.0f))
		return 0;

	//UE_LOG(LogTemp, Log, TEXT("in funct - value to add: %d"), value);
	Credits += delta;
	//UE_LOG(LogTemp, Log, TEXT("in funct - new credit total: %d"), Credits);
	//we might have a cap on how many you can get so for just return true by default
	OnCreditsChanged.Broadcast(this, Credits, delta);
	return Credits;
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		SaveObject->Credits = Credits;
	}

}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		Credits = SaveObject->Credits;
	}

}
