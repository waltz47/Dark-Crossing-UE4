// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "const.h"
#include "eveCharacter.generated.h"

UCLASS()
class EVE_API AeveCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AeveCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float maxHealth;
	float CurrentHealthPercent() const;

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float health = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float damage = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float armor = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float visionRange = 2000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float visionDeg = 60.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) FResource res;

	FTimerHandle deathTimer;

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveForward(float axis_val);
	void MoveRight(float axis_val);
	virtual void OnDeath();
	void DestroyObject();
	UFUNCTION(BlueprintCallable) bool IsDead();
	virtual float TakeDamage(float dmg, const struct FDamageEvent& eevent, AController* causer, AActor* actor) override;

};
