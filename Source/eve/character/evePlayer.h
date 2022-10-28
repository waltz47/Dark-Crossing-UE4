// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "character/eveNICharacter.h"
#include "evePlayer.generated.h"

#define WEAPON_RANGE		20000.f

class APlayerController;
UCLASS()
class EVE_API AevePlayer : public AeveNICharacter
{
	GENERATED_BODY()
private:
	UPROPERTY()
		APlayerController*							m_controller = nullptr;

protected:
	virtual void BeginPlay() override;

public:
	AevePlayer();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class UCameraComponent* camera = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class USpringArmComponent* cameraBoom = nullptr;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void StopShooting();
	void StartShooting();
	virtual void OnDeath();

	FTimerHandle m_shootTimerHandle;
};
