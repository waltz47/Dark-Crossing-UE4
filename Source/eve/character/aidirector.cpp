// Fill out your copyright notice in the Description page of Project Settings.


#include "character/aidirector.h"
#include "character/eveinfected.h"
#include "lib.h"
#include "const.h"
#include "gameframework/character.h"

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
}

void Aaidirector::Activate()
{
	if (spawnPoints.Num() == 0)
		return;
	if(bAttackGroup)
		squadToAttack = Ulib::GetAllNI(this);
	GetWorld()->GetTimerManager().SetTimer(m_spawnTimerHandle, this, &Aaidirector::SpawnAI, spawnTimeDiff, true, 0.f);
	if (bAttackGroup) {
		GetWorld()->GetTimerManager().SetTimer(t_eval_timer, this, &Aaidirector::EvalAI, m_evalTime, true, 0.f);
	}
}
void Aaidirector::SpawnAI()
{
	if (infectedClass.Num() == 0)
		return;
	if (m_currentSpawned >= numInfected) {
		GetWorld()->GetTimerManager().ClearTimer(m_spawnTimerHandle);
		return;
	}
	if (m_top >= spawnPoints.Num())
		m_top = 0;

	FVector spawn_loc = GetActorLocation() + spawnPoints[m_top];
	FRotator spawn_rot = spawn_loc.Rotation();
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AeveInfected* infected = Cast<AeveInfected>(GetWorld()->SpawnActor<ACharacter>(infectedClass[FMath::RandRange(0, infectedClass.Num() - 1)], spawn_loc, spawn_rot, spawnParams));
	if (Ulib::Valid(infected)) {
		m_infected.Add(infected);
		m_currentSpawned++;
		m_top++;
	}
}
void Aaidirector::EvalAI()
{
	int32 t_valid = 0;
	for (AeveInfected* infected : m_infected) {
		if (Ulib::Valid(infected)) {
			t_valid++;
			ACharacter* t_ctr = nullptr;
			for (ACharacter* character : squadToAttack) {
				if (Ulib::Valid(character)) {
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
	}
	if (t_valid == 0) {
		StopEval();
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