// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "SInteractionActorComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include <ActionRougeLike/Public/SAttributeComponent.h>

#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	EffectSpellCastComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectSpellCastComp->SetupAttachment(RootComponent);

	InteractiveComp = CreateDefaultSubobject<USInteractionActorComponent>("InteractionComp");

	AttributeComponent = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	//rotate to whatever we are moving towards
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	//PlayerController = GetWorld()->GetFirstPlayerController();

	bUseControllerRotationYaw = false;

	//get our material instance for flash mat
	
	HandSocketName = "Muzzle_01";

	TimeToHitParameterName = "TimeToHit";
}


void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Add dynamic is known for not being found through unreal magic (AKA INTELLISENSE), its ok that it doesnt see it here
	AttributeComponent->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const FName forward = "MoveForward";
	PlayerInputComponent->BindAxis(forward, this, &ASCharacter::MoveForward);
	const FName right = "MoveRight";
	PlayerInputComponent->BindAxis(right, this, &ASCharacter::MoveRight);

	const FName turn = "Turn";
	PlayerInputComponent->BindAxis(turn, this, &APawn::AddControllerYawInput);
	const FName lookup = "Lookup";
	PlayerInputComponent->BindAxis(lookup, this, &APawn::AddControllerPitchInput);

	const FName jump = "Jump";
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

	const FName blackhole = "Blackhole";
	PlayerInputComponent->BindAction("Blackhole",IE_Pressed, this, &ASCharacter::BlackholeAttack);

	const FName teleportAbility = "TeleportAbility";
	PlayerInputComponent->BindAction("TeleportAbility", IE_Pressed, this, &ASCharacter::TeleportAbility);

	//spawn projectile
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);

	//interactjion
	const FName interact = "PrimaryInteract";
	PlayerInputComponent->BindAction(interact, IE_Pressed, this, &ASCharacter::PrimaryInteract);
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	
	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("Player Death!"));
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
		

		
	}
	else if (Delta < 0.0f)                                                //Player took damange but not dead
	{
		//UE_LOG(LogTemp, Log, TEXT("On Health changed!"));
		//change material
		
			USkeletalMeshComponent* SkeletalMesh = GetMesh();
			SkeletalMesh->SetScalarParameterValueOnMaterials(TimeToHitParameterName, GetWorld()->TimeSeconds);
			//SkeletalMesh->SetParameterValueOnMaterials("HitFlashColor", HitFlashColor);
			FVector4 ColortoVector = HitFlashColor;
			SkeletalMesh->SetVectorParameterValueOnMaterials("HitFlashColor", ColortoVector);
			
			SkeletalMesh->SetScalarParameterValueOnMaterials("HitFlashSpeed", HitFlashSpeed);

	}
}

void ASCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackAnim);


	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElasped, 0.2f);

	


}

void ASCharacter::PrimaryInteract()
{
	if(InteractiveComp)
	{
		InteractiveComp->PrimaryInteract();
	}
	
}

//function that is called once time is done (for timer)
void ASCharacter::PrimaryAttack_TimeElasped()
{
	SpawnProjectile(ProjectileClass);



}

//movement
void ASCharacter::MoveForward(float value)
{
	FRotator ControlBot = GetControlRotation();
	ControlBot.Pitch = 0.0f;
	ControlBot.Roll = 0.0f;
	AddMovementInput(ControlBot.Vector(), value);


}
void ASCharacter::MoveRight(float value)
{
	FRotator ControlBot = GetControlRotation();
	ControlBot.Pitch = 0.0f;
	ControlBot.Roll = 0.0f;

	// X forward (red), Y is right (Green), z is up (blue) (unlike unity)

	const FVector RightVector = FRotationMatrix(ControlBot).GetScaledAxis(EAxis::Y);

	AddMovementInput(RightVector, value);
}

void ASCharacter::SpawnProjectile(TSubclassOf <AActor> classToSpawn)
{
	if (ensureAlways(classToSpawn))
	{
		//use handsocketname instead of muzzle_01
		const FVector HandLocation = GetMesh()->GetSocketLocation(HandSocketName);

		//EPSCPoolMethod PoolingMethod; //defaults to none
		//attach location can be KeepWorldPosition or KeepRelativeOffset
		// scale shouldnt be needed if we use keepworldposition
		
		//have particle effect in players hand when shooting projectile
		//should be where our hand socket is, where the projectile spawns in
		// cast root component as a scene component?
		//UParticleSystem* EmitterTemplate, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bAutoDestroy, EPSCPoolMethod PoolingMethod, bool bAutoActivateSystem
		// instructor used CastSpellVFX, GetMesh,HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget
		//UGameplayStatics::SpawnEmitterAttached(CastSpellVFX, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
		//UGameplayStatics::SpawnEmitterAttached(CastSpellVFX, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true, EPSCPoolMethod::None, true);
		UGameplayStatics::SpawnEmitterAttached(CastSpellVFX, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;


		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);


		//for trace
		float basicTraceRange = 5000;

		//the crosshair is at the forward vector of camera so might as well just use that
		FVector TraceStart = CameraComp->GetComponentLocation();

		//endpoint far into the look at distance (not to far, still somewhat towards crosshair on a miss)
		FVector TraceEnd = TraceStart + (GetControlRotation().Vector() * basicTraceRange);
		
		FHitResult hitCam;
		//if we dont hit anything just use the end of the trace line
		FVector projectileEndLocale = TraceEnd;
		//if(GetWorld()->LineTraceSingleByObjectType(hitCam, TraceStart, TraceEnd, ObjectQueryParams))
		if (GetWorld()->SweepSingleByObjectType(hitCam, TraceStart, TraceEnd, FQuat::Identity, ObjectQueryParams, Shape, Params))
		{
			//if we got a hit, then we now have a start (handlocation), and end point (hitCam.ImpactPoint)
			projectileEndLocale = hitCam.ImpactPoint;
		}

		//OLD - rotation is looking at that point we now have
		//FRotator ProjectileSpawnRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, projectileEndLocale);

		//more accurate version
		FRotator ProjRotation = FRotationMatrix::MakeFromX(projectileEndLocale - HandLocation).Rotator();


		//replaced GetControlRotation with our new target rotation
		const FTransform SpawnTM = FTransform(ProjRotation, HandLocation);
		GetWorld()->SpawnActor<AActor>(classToSpawn, SpawnTM, SpawnParams);
	}
}

//blackhole assignment 2
void ASCharacter::BlackholeAttack()
{
	UE_LOG(LogTemp, Log, TEXT("BLACKHOLE ATTACK!"));
	//UE_LOG(LogTemp, Warning, TEXT("BH ATtACK!"));
	//copy down primary attack once we see the print

	PlayAnimMontage(AttackAnim);


	GetWorldTimerManager().SetTimer(TimerHandle_BlackholeAttack, this, &ASCharacter::BlackholeAttack_TimeElasped, 0.2f);

}

//might make the spawnable projectile a passable var to make this more optimized than copying a function
//function that is called once time is done (for timer)
void ASCharacter::BlackholeAttack_TimeElasped()
{
	SpawnProjectile(BlackholeProjectileClass);

}

void ASCharacter::TeleportAbility()
{
	UE_LOG(LogTemp, Log, TEXT("Teleport Ability!"));
	//UE_LOG(LogTemp, Warning, TEXT("BH ATtACK!"));
	//copy down primary attack once we see the print

	PlayAnimMontage(AttackAnim);


	GetWorldTimerManager().SetTimer(TimerHandle_TeleportAttack, this, &ASCharacter::TeleportAbility_TimeElasped, 0.2f);

}

void ASCharacter::TeleportAbility_TimeElasped()
{
	SpawnProjectile(TeleportProjectileClass);

}

//cheat
void ASCharacter::HealSelf(float Amount /*= 100  */)
{
	AttributeComponent->ApplyHealthChange(this, Amount);

}

//override from 
FVector ASCharacter::GetPawnViewLocation() const
{
	//Super::GetPawnViewLocation();

	return CameraComp->GetComponentLocation(); //FVector();
}