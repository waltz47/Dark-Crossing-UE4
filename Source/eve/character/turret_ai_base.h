// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "character/eveCharacter.h"
#include "turret_ai_base.generated.h"

class AeveCharacter;
#define SEARCH_TIME		0.2f
UCLASS()
class EVE_API Aturret_ai_base : public AeveCharacter
{
	GENERATED_BODY()
private:
	UPROPERTY() AeveCharacter*	m_target = nullptr;
protected:
	virtual void BeginPlay() override;

public:
	Aturret_ai_base();
	virtual void Tick(float dT) override;
	virtual void OnDeath() override;
	void		Shoot();
	void		Search();
	void		SetActive();
	bool		ValidTarget();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* turretBarrel = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* turretBase = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float attackRange = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float fireRate = 2.f;

	float		m_gameTime = 0.f;
	float		m_lastShotAt = -999.f;
};
