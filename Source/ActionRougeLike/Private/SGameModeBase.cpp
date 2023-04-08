// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include <ActionRougeLike/Public/AI/SAICharacter.h>
#include <ActionRougeLike/Public/SAttributeComponent.h>
#include "EngineUtils.h"
//dont think the eenvqueryrunmode enum is needed to include header


ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	MaxBotCount = 1.0f; //10.0f;
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();


	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}



void ASGameModeBase::SpawnBotTimerElapsed()
{
	int32 NrOfAliveBots = 0;
	//
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;

		//will eventually as is alive function
		//if (Bot->Alive())
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		//we added this ensure while debugging to show that when we immedtately play we see this hit becasue the attributecomp is still null
		// never actually added teh attrubute comp to our AI yet
		// never would increase our count
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			NrOfAliveBots++;
		}
	}

	//UE_LOG(LogTemp, Log, TEXT("Found %i alive bots."), NrOfAliveBots);

	//float MaxBotCount = 1.0f; //10.0f;
	if (DifficultyCurve)
	{
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if (NrOfAliveBots >= MaxBotCount)
	{
		//UE_LOG(LogTemp, Log, TEXT("At max bot count, skipping bot spawn"));
		return;
	}


	//eqs query for spawning bots
	//using old style of enums (dont really use this much anymore)
	//random best 5%
	//never really use the last param
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

	QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryCompleted);

}

void ASGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	//not using ensure because it would be triggering every 2 frames and having this check is instantatious (more useful this way)
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Bot EQS Query Failed!"));
		return;
	}

	//give us the result and location for where we want to spawn
	//status tells us if it fails, etc
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	
	//check our locations
	if (Locations.IsValidIndex(0))
	{

		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);

		//track all used spawn locations
		DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
	}
}

//cheat
void ASGameModeBase::KillAll()
{
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this); //@fixme pass in player? for kill credit
		}
	}
}