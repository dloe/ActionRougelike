// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "SAttributeComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
//#include "Camera/CameraShakeBase.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{

	//UE_LOG(LogTemp, Log, TEXT("check"));
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);

	myRadialForce = CreateDefaultSubobject<URadialForceComponent>("myRadialForce");
	myRadialForce->Radius = 700;
	myRadialForce->ImpulseStrength = 1000;
	//optional, ignore 'Mass' of other objects (if false, the impulse strength will be much higher to push most objects depending on mass)
	myRadialForce->bImpulseVelChange = false;
	myRadialForce->SetupAttachment(RootComponent);

	AudioCompFlight->Play(0.0f);
	Damage = 10;
	
}
//UPrimitiveComponent* OnComponentBeginOverlap, 
void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && OtherActor != GetInstigator())
	{
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));
		if(AttributeComp)
		{
			//instead of passing this object, lets pass instigator since it should be more useful because it should be the player or the minion
			AttributeComp->ApplyHealthChange(GetInstigator(), - Damage);

			//spawn emmiter at location
			UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

			Destroy();
		}

		//UE_LOG(LogTemp, Log, TEXT("OnActorOverlap in projectile"));

	}
}

void ASMagicProjectile::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnActorHit(HitComp, OtherActor, OtherComp,NormalImpulse, Hit);
	//double check this with answer
	//FString temp = OtherActor->GetFName().ToString();
	//UE_LOG(LogTemp, Log, TEXT("Other actor: s%"), temp);
	//if (OtherActor && OtherActor != GetInstigator())
	//{
		UE_LOG(LogTemp, Log, TEXT("OnCompHit in projectile"));
		//if we hit anything else (logic ported from blueprint)

		DrawDebugSphere(GetWorld(), GetActorLocation(), 100, 12, FColor::White, 1.0f, 0.0f);

		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

		UGameplayStatics::PlaySoundAtLocation(GetWorld(),ImpactSound, GetActorLocation(),GetActorRotation());

		//camera shake on projectile impact
		//add camera shake at a certain location in the world then look for players within their affected radius and apply 
		//the scale based on how close they are
		//const UObject* WorldContextObject, TSubclassOf<class UCameraShakeBase> Shake, FVector Epicenter, float InnerRadius, float OuterRadius, float Falloff, bool bOrientShakeTowardsEpicenter
		
		//establish radius for myshake
		//make outer and inner radius float vars editable via inspector
		UGameplayStatics::PlayWorldCameraShake(this, MyShake, Hit.ImpactPoint, ImpactShakeInnerRadius, ImpactShakeOuterRadius, Falloff);

		Destroy();
	//}
}

