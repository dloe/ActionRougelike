// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "AI/SAIController.h"
#include "BehaviorTree/BlackboardComponent.h"


// Sets default values
ASAICharacter::ASAICharacter()
{
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

}

void ASAICharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
    AAIController* AIC = Cast<AAIController>(GetController());

    if (AIC)
    {
        //dont even need to null check this since we know for a fact this is valid. Cannot be null
        UBlackboardComponent* BBComp = AIC->GetBlackboardComponent();

        //for now we will keep them hard coded values
        BBComp->SetValueAsObject("TargetActor", Pawn);

        //add a draw debug string at the location of the actor so that we have something that shows where the player was spotted
        DrawDebugString(GetWorld(), GetActorLocation(), "PlayerSpotted", nullptr, FColor::White, 4.0f, true);
    }

}



