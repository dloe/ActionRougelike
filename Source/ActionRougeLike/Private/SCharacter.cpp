// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "SInteractionActorComponent.h"
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

	InteractiveComp = CreateDefaultSubobject<USInteractionActorComponent>("InteractionComp");

	AttributeComponent = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	//rotate to whatever we are moving towards
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	//PlayerController = GetWorld()->GetFirstPlayerController();

	bUseControllerRotationYaw = false;
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
	float basicTraceRange = 5000; 
	const FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	//the crosshair is at the forward vector of camera so might as well just use that
	FVector TraceStart = CameraComp->GetComponentLocation();
	FVector TraceEnd = TraceStart + (GetControlRotation().Vector() * basicTraceRange);

	//for trace

	FCollisionShape Shape;
	Shape.SetSphere(20.0f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	FHitResult hitCam;

	//if we dont hit anything just use the end of the trace line
	FVector projectileEndLocale = TraceEnd;
	//if(GetWorld()->LineTraceSingleByObjectType(hitCam, TraceStart, TraceEnd, ObjectQueryParams))
	if(GetWorld()->SweepSingleByObjectType(hitCam, TraceStart, TraceEnd, FQuat::Identity, ObjectQueryParams, Shape, Params))
	{
		//if we got a hit, then we now have a start (handlocation), and end point (hitCam.ImpactPoint)
		projectileEndLocale = hitCam.ImpactPoint;
	}

	//rotation is looking at that point we now have
	FRotator ProjectileSpawnRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, projectileEndLocale);

	//replaced GetControlRotation with our new target rotation
	const FTransform SpawnTM = FTransform(ProjectileSpawnRotation, HandLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);


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
	float basicTraceRange = 5000;
	const FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	//the crosshair is at the forward vector of camera so might as well just use that
	FVector TraceStart = CameraComp->GetComponentLocation();
	FVector TraceEnd = TraceStart + (GetControlRotation().Vector() * basicTraceRange);

	//for trace

	FCollisionShape Shape;
	Shape.SetSphere(20.0f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	FHitResult hitCam;

	//if we dont hit anything just use the end of the trace line
	FVector projectileEndLocale = TraceEnd;
	//if(GetWorld()->LineTraceSingleByObjectType(hitCam, TraceStart, TraceEnd, ObjectQueryParams))
	if (GetWorld()->SweepSingleByObjectType(hitCam, TraceStart, TraceEnd, FQuat::Identity, ObjectQueryParams, Shape, Params))
	{
		//if we got a hit, then we now have a start (handlocation), and end point (hitCam.ImpactPoint)
		projectileEndLocale = hitCam.ImpactPoint;
	}

	//rotation is looking at that point we now have
	FRotator ProjectileSpawnRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, projectileEndLocale);

	//replaced GetControlRotation with our new target rotation
	const FTransform SpawnTM = FTransform(ProjectileSpawnRotation, HandLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	GetWorld()->SpawnActor<AActor>(BlackholeProjectileClass, SpawnTM, SpawnParams);


}

void ASCharacter::TeleportAbility()
{
	UE_LOG(LogTemp, Log, TEXT("Teleport Ability!"));
	//UE_LOG(LogTemp, Warning, TEXT("BH ATtACK!"));
	//copy down primary attack once we see the print

	PlayAnimMontage(AttackAnim);


	GetWorldTimerManager().SetTimer(TimerHandle_BlackholeAttack, this, &ASCharacter::BlackholeAttack_TimeElasped, 0.2f);

}

void ASCharacter::TeleportAbility_TimeElasped()
{
	float basicTraceRange = 5000;
	const FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	//the crosshair is at the forward vector of camera so might as well just use that
	FVector TraceStart = CameraComp->GetComponentLocation();
	FVector TraceEnd = TraceStart + (GetControlRotation().Vector() * basicTraceRange);

	//for trace

	FCollisionShape Shape;
	Shape.SetSphere(20.0f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	FHitResult hitCam;

	//if we dont hit anything just use the end of the trace line
	FVector projectileEndLocale = TraceEnd;
	//if(GetWorld()->LineTraceSingleByObjectType(hitCam, TraceStart, TraceEnd, ObjectQueryParams))
	if (GetWorld()->SweepSingleByObjectType(hitCam, TraceStart, TraceEnd, FQuat::Identity, ObjectQueryParams, Shape, Params))
	{
		//if we got a hit, then we now have a start (handlocation), and end point (hitCam.ImpactPoint)
		projectileEndLocale = hitCam.ImpactPoint;
	}

	//rotation is looking at that point we now have
	FRotator ProjectileSpawnRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, projectileEndLocale);

	//replaced GetControlRotation with our new target rotation
	const FTransform SpawnTM = FTransform(ProjectileSpawnRotation, HandLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	//sub in our teleport projectile
	GetWorld()->SpawnActor<AActor>(BlackholeProjectileClass, SpawnTM, SpawnParams);
}