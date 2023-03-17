// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthPotion.h"
#include "GameFramework/Actor.h"
#include "SAttributeComponent.h"

// Sets default values
ASHealthPotion::ASHealthPotion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthIncrease = 10;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;
	// disable collision, instead we use sphere comp to handle interaction queries
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ASHealthPotion::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASHealthPotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	//make sure our instiator
	if (!ensure(InstigatorPawn))// or InstigatorPawn && InstigatorPawn != GetInstigator())
	{
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
		if (ensure(AttributeComp) && AttributeComp->IsUnderMaxHealth() && !Triggered)
		{
			//if players health is below max AND hasnt already been triggered within 10 sec

				UE_LOG(LogTemp, Log, TEXT("Health Potion: Triggered!"));
				//start timer for 10 seconds, trigger = true (once timer ends, set bool and mesh)
				Triggered = true;
				ShowPowerup(false);

				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASHealthPotion::OnTriggerTimer, TriggerDelay);
			
				AttributeComp->ApplyHealthChange(this, HealthIncrease);
		}
	}

}

//runs when our timer ends
void ASHealthPotion::OnTriggerTimer()
{
	UE_LOG(LogTemp, Log, TEXT("No longer triggered..."));
	//set bool and mesh
	Triggered = false;

	ShowPowerup(true);

	GetWorldTimerManager().ClearTimer(TimerHandle);
}

