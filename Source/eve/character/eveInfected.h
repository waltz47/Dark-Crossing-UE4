// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "character/eveCharacter.h"
#include "eveInfected.generated.h"

#define LOOK_AT_TOLERANCE	10.f
#define MELEE_RANGE 150.f
#define NUM_RANDOM_POINTS	10

enum InfectedState;
enum StatusEffect;
class AAIController;
class UAnimSequence;
class AeveCharacter;
UCLASS()
class EVE_API AeveInfected : public AeveCharacter
{
	GENERATED_BODY()

private:
	float							m_gameTime = 0.f;
	float							m_lastStatusDamageSnap = -999.f;
	InfectedState					m_infectedState;
	StatusEffect					m_statusEffect;
	UPROPERTY() AAIController*		m_aicontroller = nullptr;
	UPROPERTY()	AeveCharacter*		m_target = nullptr;
	UPROPERTY()	UParticleSystemComponent*	m_status_fire_particle = nullptr;

	FTimerHandle					m_attackTimerHandle;
protected:
	virtual void BeginPlay() override;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool							m_bAttacking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float roamSpeed = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float chaseSpeed = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float statusDamageTick = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float fireDamage = 10.f;

	AeveInfected();
	virtual void Tick(float DeltaTime) override;
	InfectedState GetInfectedState() { return m_infectedState; }
	void SetInfectedState(InfectedState t_newState);
	void AddStatusEffect(StatusEffect t_effect, UParticleSystem* t_particle);
	StatusEffect GetStatusActive() { return m_statusEffect; }
	void MeleeAttack();
	UFUNCTION(BlueprintCallable)
		void Attack_Over();

	bool m_bDeadFlag = 0;

	virtual float TakeDamage(float _damage, const struct FDamageEvent& damagEvent, AController* instigator, AActor* causer);
	virtual void OnDeath() override;


	friend class Aaidirector;
};
