// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "character/eveNICharacter.h"
#include "evePed.generated.h"

class USoundCue;
class AevePlayer;
class AeveInfected;
class Aweapon;
class AAIController;
enum PedState;

#define VALID_TARGET_TOLERANCE	5000.f
#define MOVE_TO_TOLERANCE		300.f

UCLASS()
class EVE_API AevePed : public AeveNICharacter
{
	GENERATED_BODY()
	
private:
	PedState										m_pedState;
	UPROPERTY() AAIController*						m_aicontroller = nullptr;
	UPROPERTY()	AActor*								m_target = nullptr;
	UPROPERTY()	AActor*								m_squadMember = nullptr;
	float											m_lastShootTime = -999.f;
	float											m_gameTime = 0.f;
	FRotator										m_lookAtRot = FRotator::ZeroRotator;
	float											m_lastLookAtTime = -999.f;
	float											m_lookWaitTolerance = 4.f;
	void SetPedState(PedState t_newState);

	// Death sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound, meta=(AllowPrivateAccess = "true"))
	USoundCue* DeathSoundCue;

protected:
	virtual void BeginPlay() override;
	virtual void OnDeath() override;
	
public:
	AevePed();
	bool		IsCurrentTargetValid();
	void		TryShoot();
	virtual void Tick(float DeltaTime) override;
};
