// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthPotion.h"
#include "GameFramework/Actor.h"
#include "SAttributeComponent.h"

// Sets default values
ASHealthPotion::ASHealthPotion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Triggered = false;
	TriggerDelay = 10;
	HealthIncrease = 10;
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
	if (InstigatorPawn && InstigatorPawn != GetInstigator())
	{
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
		if (AttributeComp)
		{
			//if players health is below max AND hasnt already been triggered within 10 sec
			if (AttributeComp->IsUnderMaxHealth() && !Triggered) {
				UE_LOG(LogTemp, Log, TEXT("Health Potion: Triggered!"));
				//start timer for 10 seconds, trigger = true (once timer ends, set bool and mesh)
				Triggered = true;
				//set mesh invisible and turn off interaction
				BaseMesh->SetVisibility(false);

				//turn off generate overlap events
				//BaseMesh->SetActorEnableCollision(false);
				SetActorEnableCollision(false);
				
				

				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASHealthPotion::OnTriggerTimer, TriggerDelay);
			
				AttributeComp->ApplyHealthChange(HealthIncrease);
			
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Health Potion: Cant be triggered!"));
			}
		}
	}

}

//runs when our timer ends
void ASHealthPotion::OnTriggerTimer()
{
	UE_LOG(LogTemp, Log, TEXT("No longer triggered..."));
	//set bool and mesh
	Triggered = false;

	//set mesh invisible and turn off interaction!
	//BaseMesh->SetStaticMesh(NormalPotionMesh);
	BaseMesh->SetVisibility(true);

	//turn on overlap events
	SetActorEnableCollision(true);

	GetWorldTimerManager().ClearTimer(TimerHandle);
}

