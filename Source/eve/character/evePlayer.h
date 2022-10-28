// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "character/eveNICharacter.h"
#include "evePlayer.generated.h"

#define WEAPON_RANGE		20000.f

class APlayerController;
class Aturret_ai_base;
UCLASS()
class EVE_API AevePlayer : public AeveNICharacter
{
	GENERATED_BODY()
private:
	UPROPERTY()
		APlayerController*							m_controller = nullptr;

	UPROPERTY()
		Aturret_ai_base*							m_placingObj = nullptr;
	bool											m_bPlacingObjLocationValid = 0;

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
	UFUNCTION(BlueprintCallable) void SetPlacingObject(Aturret_ai_base* t_obj);

	FTimerHandle m_shootTimerHandle;
};
