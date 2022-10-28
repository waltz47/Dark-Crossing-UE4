// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "aidirector.generated.h"

class ACharacter;
class AeveInfected;
UCLASS()
class EVE_API Aaidirector : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	int32					m_currentSpawned = 0;
	int32					m_top = 0;
	int32					m_squadtop = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool					bAttackGroup = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32					numInfected = 40;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32					maxSimActive = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float					spawnTimeDiff = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32					m_evalTime = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TSubclassOf<AeveInfected>> infectedClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (MakeEditWidget = true))
		TArray<FVector>			spawnPoints;

	FTimerHandle		m_spawnTimerHandle;
	UPROPERTY() TArray<ACharacter*>		squadToAttack;
	UPROPERTY()	TArray<AeveInfected*>	m_infected;

	Aaidirector();
	void Activate();
	void SpawnAI();
	void EvalAI();
	void StopEval();
	FTimerHandle t_eval_timer;


};
