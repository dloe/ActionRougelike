// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAttributeComponent.h"
#include "SInteractionActorComponent.h"
#include "GameFramework/Character.h"
#include "Particles/ParticleSystemComponent.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USInteractionComponent;
class UAnimMontage;

UCLASS()
class ACTIONROUGELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	
	UPROPERTY(EditAnywhere, Category = "Attack")
		TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
		TSubclassOf<AActor>BlackholeProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
		TSubclassOf<AActor>TeleportProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
		UAnimMontage* AttackAnim;

	//attack timers
	FTimerHandle TimerHandle_PrimaryAttack;
	FTimerHandle TimerHandle_BlackholeAttack;
	FTimerHandle TimerHandle_TeleportAttack;

	//particle effects on spell spawn
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* CastSpellVFX;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* EffectSpellCastComp;


	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	//interaction
	UPROPERTY(VisibleAnywhere)
	USInteractionActorComponent* InteractiveComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributeComponent;

	UPROPERTY(EditDefaultsOnly)
	int HitFlashSpeed;

	//UPROPERTY(EditDefaultsOnly)
	//FVector4 HitFlashColor;

	UPROPERTY(EditDefaultsOnly)
	FColor HitFlashColor;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName HandSocketName;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float value);
	void MoveRight(float value);

	void SpawnProjectile(TSubclassOf<AActor> classToSpawn);

	void BlackholeAttack();

	void TeleportAbility();

	void TeleportAbility_TimeElasped();

	void BlackholeAttack_TimeElasped();

	void PrimaryAttack();

	void PrimaryInteract();

	void PrimaryAttack_TimeElasped();

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);


	//slightly better way to bind your events in timeline
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



};
