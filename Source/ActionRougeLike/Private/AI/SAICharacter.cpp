// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "SAttributeComponent.h"
#include "AI/SAIController.h"
#include "BrainComponent.h"
#include "SWorldUserWidget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SPlayerState.h"
#include <ActionRougeLike/Public/SGameModeBase.h>
#include <SActionComponent.h>

// Sets default values
ASAICharacter::ASAICharacter()
{
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
    AttributeComponent = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
    ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    //fix an issue with collision on our capsule collider
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
    GetMesh()->SetGenerateOverlapEvents(true);

    CreditsOnKill = 20;
    TimeToHitParameterName = "TimeToHit";
    DeathReward = 10;
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
    SetTargetActor(Pawn);
    //add a draw debug string at the location of the actor so that we have something that shows where the player was spotted
    DrawDebugString(GetWorld(), GetActorLocation(), "PlayerSpotted", nullptr, FColor::White, 4.0f, true);
    

}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
    if (Delta < 0.0f)
    {
        //if we shoot at the AI the AI needs to know that someone shot at them and whoever shot our AI needs to become the designated target
        //theres a chance that an AI might shoot a friendly AI and they might agro on each other, will keep for now but might remove later
        if (InstigatorActor)
        {
            SetTargetActor(InstigatorActor);
        }

        if (ActiveHealthBar == nullptr) {
            //spawn ui, only spawn when there isnt already a widget
            ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
            //add it to viewport
            if (ActiveHealthBar)
            {
                //set attached actor (run event constructor)
                ActiveHealthBar->AttachedActor = this;
                ActiveHealthBar->AddToViewport();
            }
        }

        //hit flash
        GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParameterName, GetWorld()->TimeSeconds);

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


            //remove the stuff our ai leaves behind (like capsules, etc)
            GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            //disable this char movement completely (gravity)
            GetCharacterMovement()->DisableMovement();

            //set lifespan
            SetLifeSpan(10.0f);

            //drop currency on death (spawn coin pickup)
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            SpawnParams.Instigator = this;

            const FTransform SpawnTM = FTransform(this->GetActorRotation(), this->GetActorLocation());
            
            //on minion death event via SGameModeBase
            ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
            if (GM)
            {
                GM->KillMinionEvent(InstigatorActor, CreditsOnKill);
            }
        }
    }


    
}

void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC)
    {
        //dont even need to null check this since we know for a fact this is valid. Cannot be null
        AIC->GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);
    }

}



