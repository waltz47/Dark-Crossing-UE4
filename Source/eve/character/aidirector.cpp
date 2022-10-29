// Fill out your copyright notice in the Description page of Project Settings.


#include "character/aidirector.h"
#include "character/eveinfected.h"
#include "lib.h"
#include "const.h"
#include "gameframework/character.h"
#include "character/evePlayer.h"
#include "kismet/gameplaystatics.h"

// Sets default values
Aaidirector::Aaidirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void Aaidirector::BeginPlay()
{
	Super::BeginPlay();
	NextWave();
}

void Aaidirector::NextWave()
{
	if (spawnPoints.Num() == 0)
		return;
	
	if (m_currentWave) {
		if(!Ulib::Valid(m_player))
			m_player = Cast<AevePlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

		if (Ulib::Valid(m_player)) {
			m_player->AddGold(numInfected * KILL_MULTIPLIER);
		}
	}
	/*Increase health / armor*/
	m_currentWaveHealth += 10.f;
	m_currentWaveArmor += 5.f;
	if (m_currentWaveArmor > INF_ARMOR_CAP) {
		m_currentWaveArmor = INF_ARMOR_CAP;
	}
	m_currentWaveDamage += 5.f;
	if (m_currentWaveDamage > INF_DAMAGE_CAP) {
		m_currentWaveDamage = INF_DAMAGE_CAP;
	}

	squadToAttack = Ulib::GetAllNI(this);
	numInfected += 10;
	maxSimActive += 5;
	if (maxSimActive > INF_MAX_SIM_ACTIVE_CAP) {
		maxSimActive = INF_MAX_SIM_ACTIVE_CAP;
	}
	if (numInfected > INF_CAP)
		numInfected = INF_CAP;
	m_currentWave++;
	m_currentWaveKillCount = 0;
	m_currentSpawned = 0;
	m_top = 0;
	m_infected.Empty();
	squadToAttack.Empty();
	
	GetWorld()->GetTimerManager().SetTimer(t_next_wave_timer, this, &Aaidirector::WaveStart, waveCooldown, false);
}
void Aaidirector::WaveStart()
{
	squadToAttack = Ulib::GetAllNI(this);

	GetWorld()->GetTimerManager().SetTimer(m_spawnTimerHandle, this, &Aaidirector::SpawnAI, spawnTimeDiff, true, 0.f);
	GetWorld()->GetTimerManager().SetTimer(t_eval_timer, this, &Aaidirector::EvalAI, m_evalTime, true, 0.f);
}
void Aaidirector::SpawnAI()
{
	if (infectedClass.Num() == 0 || spawnPoints.Num() == 0)
		return;
	if (m_currentSpawned >= numInfected) {
		GetWorld()->GetTimerManager().ClearTimer(m_spawnTimerHandle);
		m_bWaveOver = 1;
		return;
	}
	m_top %= spawnPoints.Num();
	int32 tp = spawnPoints.Num();
	if (Ulib::Valid(m_player)) {
		float t_dst = FVector::Distance(m_player->GetActorLocation(), GetActorLocation() + spawnPoints[m_top]);
		while (t_dst <= 3000.f && tp-- && Ulib::Valid(m_player)) {
			m_top++;
			m_top %= spawnPoints.Num();
			t_dst = FVector::Distance(m_player->GetActorLocation(), GetActorLocation() + spawnPoints[m_top]);
		}
	}
	m_top %= spawnPoints.Num();
	FVector spawn_loc = GetActorLocation() + spawnPoints[m_top];
	FRotator spawn_rot = spawn_loc.Rotation();
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AeveInfected* infected = Cast<AeveInfected>(GetWorld()->SpawnActor<ACharacter>(infectedClass[FMath::RandRange(0, infectedClass.Num() - 1)], spawn_loc, spawn_rot, spawnParams));
	if (Ulib::Valid(infected)) {
		infected->health = m_currentWaveHealth;
		infected->armor = m_currentWaveArmor;
		infected->damage = m_currentWaveDamage;
		m_infected.Add(infected);
		m_currentSpawned++;
		m_top++;
	}
}
void Aaidirector::EvalAI()
{
	int32 t_valid = 0;
	m_currentWaveKillCount = 0;
	for (AeveInfected* infected : m_infected) {
		if (Ulib::Valid(infected) && infected->IsDead() == false) {
			t_valid++;
			AeveCharacter* t_ctr = nullptr;
			for (AeveCharacter* character : squadToAttack) {
				if (Ulib::Valid(character) && character->IsDead()==false) {
					float t_dst = FVector::Distance(character->GetActorLocation(), infected->GetActorLocation());
					if (!Ulib::Valid(t_ctr) || t_dst < FVector::Distance(t_ctr->GetActorLocation(), infected->GetActorLocation())) {
						t_ctr = character;
					}
				}
			}
			if (Ulib::Valid(t_ctr)) {
				infected->m_target = t_ctr;
				infected->SetInfectedState(INFECTED_STATE_CHASE);
			}
		}
		else m_currentWaveKillCount++;
	}
	if (t_valid == 0 && m_bWaveOver) {
		//wave over
		StopEval();
		m_overallSpawned += numInfected;
		NextWave();
		return;
	}
	if (t_valid > maxSimActive) {
		GetWorld()->GetTimerManager().PauseTimer(m_spawnTimerHandle);
	}
	else if (GetWorld()->GetTimerManager().IsTimerActive(m_spawnTimerHandle) == false) {
		GetWorld()->GetTimerManager().UnPauseTimer(m_spawnTimerHandle);
	}
	//UE_LOG(LogTemp, Warning, TEXT("num inf: %d"), t_valid);
}

void Aaidirector::StopEval()
{
	GetWorld()->GetTimerManager().ClearTimer(t_eval_timer);
}