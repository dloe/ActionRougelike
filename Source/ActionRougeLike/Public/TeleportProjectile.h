// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SMagicProjectile.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleportProjectile.generated.h"


UCLASS()
class ACTIONROUGELIKE_API ATeleportProjectile : public ASMagicProjectile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleportProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnActorOverlapTeleport(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
