// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionActorComponent.h"

#include "SGameplayInterface.h"
#include "VT/VirtualTextureBuildSettings.h"

static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.InteractionDebugDraw"), false, TEXT("Enable Debug lines for Interact Command."), ECVF_Cheat);

// Sets default values for this component's properties
USInteractionActorComponent::USInteractionActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void USInteractionActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void USInteractionActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USInteractionActorComponent::PrimaryInteract()
{
	bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	AActor* MyOwner = GetOwner();

	FVector Start;
	FVector End;

	FVector EyeLocation;
	FRotator EyeRotation;
	//this can cause a bug with interacting based on our char eye dir and not our crosshair
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	End = EyeLocation + (EyeRotation.Vector() * 1000); //CameraComp->GetComponentLocation() + (GetControlRotation().Vector() * 1000);//EyeLocation + (EyeRotation.Vector() * 1000);

	//v1
	//FHitResult hit;
	//start and end (using eyelocal for start)
	//bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(hit, EyeLocation, End, ObjectQueryParams);


	//ver 2
	float radius = 30.0f;
	TArray<FHitResult> Hits;
	FCollisionShape shape;
	shape.SetSphere(radius);
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, shape);


	for(FHitResult hit : Hits)
	{
		AActor* HitActor = hit.GetActor();
		if (HitActor)
		{
			FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
			if (bDebugDraw)
				DrawDebugSphere(GetWorld(), hit.ImpactPoint, radius, 32, LineColor, false, 2.0f);
			if (HitActor->Implements<USGameplayInterface>())
			{
				APawn* MyPawn = Cast<APawn>(MyOwner);
				//is ok if null ptr 

				ISGameplayInterface::Execute_Interact(HitActor, MyPawn);

				//break out of for loop
				break;
			}
		}
	}

	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
	if (bDebugDraw)
		DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 2.0f);
}
