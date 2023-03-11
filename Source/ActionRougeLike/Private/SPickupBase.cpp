// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupBase.h"
#include "SAttributeComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "SGameplayInterface.h"


// Sets default values
ASPickupBase::ASPickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionProfileName("Powerup");
	RootComponent = SphereComp;

	

	//HealthIncrease
	Triggered = false;
	TriggerDelay = 10;
}

// Called when the game starts or when spawned
void ASPickupBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASPickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//when player interacts with our health potion
void ASPickupBase::Interact_Implementation(APawn* InstigatorPawn)
{
	//make sure our instiator
	if (InstigatorPawn && InstigatorPawn != GetInstigator())
	{
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
		if (AttributeComp)
		{
			//base logic (could branch this off into a new function our children can override, then we dont have to do this few line setup


		}
	}
}

void ASPickupBase::OnTriggerTimer()
{
	UE_LOG(LogTemp, Log, TEXT("No longer triggered..."));
	//set bool and mesh
	Triggered = false;

	//set mesh invisible and turn off interaction!
	//BaseMesh->SetStaticMesh(NormalPotionMesh);
	BaseMesh->SetVisibility(true, true);

	//turn on overlap events
	SetActorEnableCollision(true);

	GetWorldTimerManager().ClearTimer(TimerHandle);
}

void ASPickupBase::ShowPowerup(bool state)
{
	//set mesh invisible and turn off interaction!
	//BaseMesh->SetStaticMesh(NormalPotionMesh);
	BaseMesh->SetVisibility(state, state);

	//turn on overlap events
	SetActorEnableCollision(state);

}
