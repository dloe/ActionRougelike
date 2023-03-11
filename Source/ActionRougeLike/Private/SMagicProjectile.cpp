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
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Projectile");
	//SphereComp->Simulat
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
	SphereComp->OnComponentHit.AddDynamic(this, &ASMagicProjectile::OnCompHit);
	RootComponent = SphereComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 1000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;


	myRadialForce = CreateDefaultSubobject<URadialForceComponent>("myRadialForce");
	myRadialForce->Radius = 700;
	myRadialForce->ImpulseStrength = 1000;
	//optional, ignore 'Mass' of other objects (if false, the impulse strength will be much higher to push most objects depending on mass)
	myRadialForce->bImpulseVelChange = false;
	myRadialForce->SetupAttachment(RootComponent);

	AudioCompFlight = CreateDefaultSubobject<UAudioComponent>("AudioComp");
	AudioCompFlight->SetupAttachment(RootComponent);
	AudioCompFlight->Play(0.0f);
	
}
//UPrimitiveComponent* OnComponentBeginOverlap, 
void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && OtherActor != GetInstigator())
	{
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));
		if(AttributeComp)
		{
			AttributeComp->ApplyHealthChange(-Damage);

			//spawn emmiter at location
			UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

			Destroy();
		}

		//UE_LOG(LogTemp, Log, TEXT("OnActorOverlap in projectile"));

	}
}

void ASMagicProjectile::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
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
		UGameplayStatics::PlayWorldCameraShake(this, MyShake, Hit.ImpactPoint, InnerRadius, OuterRadius, Falloff);

		Destroy();
	//}
}

// Called when the game starts or when spawned
void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASMagicProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Called every frame
void ASMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

