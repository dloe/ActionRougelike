// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/PlayerState.h"
#include "SGameModeBase.generated.h"

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCreditChange, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewCredits, float, Delta);

UCLASS()
class ACTIONROUGELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:

	//ai
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> MinionClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;

	FTimerHandle TimerHandle_SpawnBots;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnTimerInterval;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float MaxBotCount;

	//env
	UPROPERTY(EditDefaultsOnly, Category = "Env")
	TArray<TSubclassOf<AActor>> PowerupClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Env")
	UEnvQuery* PowerupSpawnQuery;

	UPROPERTY(EditDefaultsOnly, Category = "Env")
	float MaxHealthPotionSpawnCount;

	UPROPERTY(EditDefaultsOnly, Category = "Env")
	float MaxCoinSpawnCount;

	UPROPERTY(EditDefaultsOnly, Category = "Env")
	float DesiredPowerupCount;
	UPROPERTY(EditDefaultsOnly, Category = "Env")
	float RequiredPowerupDistance;

	bool WorldSpawnSet;

	//UPROPERTY(EditDefaultsOnly, Category = "Player")
	//PlayerStateClass PlayerState;

	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	void OnQueryPowerupSpawnCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	UFUNCTION()
	void OnQueryBotSpawnCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void RespawnPlayerElasped(AController* Controller);

	UFUNCTION()
	void SpawnPowerups();

public:
	ASGameModeBase();

	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);

	virtual void StartPlay() override;

	UFUNCTION(exec)
	void KillAll();

	//when player kills minion
	UFUNCTION()
	void KillMinionEvent(AActor* InstigatorActor, int MinionCost);
};
