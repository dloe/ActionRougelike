// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "SAttributeComponent.h"
#include "AI/SAIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


// Sets default values
ASAICharacter::ASAICharacter()
{
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
    AttributeComponent = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;


}

void ASAICharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);

    //Add dynamic is known for not being found through unreal magic (AKA INTELLISENSE), its ok that it doesnt see it here
    AttributeComponent->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
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

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
    if (Delta < 0.0f)
    {


        if (NewHealth <= 0.0f)
        {
            //stop BT 
            AAIController* AIC = Cast<AAIController>(GetController());
            if (AIC)
            {
                AIC->GetBrainComponent()->StopLogic("Killed");
            }

            //rag doll
            GetMesh()->SetAllBodiesSimulatePhysics(true);
            //need to set physics and collision to prevent unwanted issues with things falling through floor, etc
            GetMesh()->SetCollisionProfileName("Ragdoll");

            //set lifespan
            SetLifeSpan(10.0f);
        }
    }

}



