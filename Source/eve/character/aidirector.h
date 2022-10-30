// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "aidirector.generated.h"

class ACharacter;
class AeveInfected;
class AeveCharacter;
class AevePlayer;
#define INF_CAP					300 //Per Wave
#define INF_ARMOR_CAP			100
#define INF_DAMAGE_CAP			80
#define INF_MAX_SIM_ACTIVE_CAP	200 //max infected at a time
#define KILL_MULTIPLIER			2

UENUM(BlueprintType)
enum class EGameDifficulty :uint8
{
	LEVEL_EASY,
	LEVEL_MEDIUM,
	LEVEL_HARD,
	LEVEL_INSANE
};
UCLASS()
class EVE_API Aaidirector : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	int32					numInfected = 40;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	int32					waveCooldown = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	int32					maxSimActive = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	float					spawnTimeDiff = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	float					AIEvaluationTime = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	float					FactorEasy = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	float					FactorMedium = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	float					FactorHard = 4.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	float					FactorInsane = 8.f;
	UPROPERTY()								AevePlayer*					m_player = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)									TArray<TSubclassOf<AeveInfected>> infectedClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (MakeEditWidget = true))	TArray<FVector>			spawnPoints;

	UPROPERTY() TArray<AeveCharacter*>	squadToAttack;
	UPROPERTY()	TArray<AeveInfected*>	m_infected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGameDifficulty			DifficultySetting = EGameDifficulty::LEVEL_MEDIUM;

	Aaidirector();
	void WaveStart();
	void SpawnAI();
	void EvalAI();
	void StopEval();
	void NextWave();
	float GetDifficultyFactor()
	{
		switch (DifficultySetting) {
			case EGameDifficulty::LEVEL_EASY: {
				return FactorEasy;
				break;
			}
			case EGameDifficulty::LEVEL_MEDIUM:
			{
				return FactorMedium;
				break;
			}
			case EGameDifficulty::LEVEL_HARD:
			{
				return FactorHard;
				break;
			}
			default:
				return FactorInsane;
		}
	}
	FTimerHandle m_spawnTimerHandle;
	FTimerHandle t_eval_timer;
	FTimerHandle t_next_wave_timer;

	int32					m_currentSpawned = 0;
	int32					m_top = 0;
	int32					m_currentWave = 0;
	int32					m_squadtop = 0;
	int32					m_overallSpawned = 0;
	int32					m_currentWaveKillCount = 0;
	float					m_currentWaveHealth = 100.f;
	float					m_currentWaveArmor = 10.f;
	float					m_currentWaveDamage = 10.f;
	bool					m_bWaveOver = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32					m_valid = 0;

	UFUNCTION(BlueprintCallable) void GetRemainingInfectedCount();
	UFUNCTION(BlueprintCallable) int32 GetTotalInfectedKilled() { return m_overallSpawned + m_currentWaveKillCount; }
	UFUNCTION(BlueprintCallable) int32 GetCurrentWaveNum() { return m_currentWave; }
	UFUNCTION(BlueprintCallable) float GetTimeToNextWave() {
		return GetWorld()->GetTimerManager().GetTimerRemaining(t_next_wave_timer);
	}


};
