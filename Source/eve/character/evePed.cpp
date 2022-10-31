// Fill out your copyright notice in the Description page of Project Settings.


#include "character/evePed.h"
#include "lib.h"
#include "const.h"
#include "kismet/gameplaystatics.h"
#include "aicontroller.h"
#include "particles/particlesystem.h"
#include "objects/weapon.h"
#include "character/evecharacter.h"
#include "sound/soundcue.h"

AevePed::AevePed()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AevePed::BeginPlay()
{
	Super::BeginPlay();
	m_aicontroller = Cast<AAIController>(GetController());
	m_squadMember = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!m_squadMember) {
		UE_LOG(LogTemp, Warning, TEXT("no squad"));
	}
	else {
		SetPedState(PED_STATE_GROUPED);
	}
	if (Ulib::Valid(m_weapPrimary)) {
		m_weapPrimary->MuzzleSoundMultiplier = 0.3f;
	}
}

void AevePed::OnDeath()
{
	Super::OnDeath();

	// Play death sound cue
	UGameplayStatics::PlaySound2D(GetWorld(), DeathSoundCue);
}

void AevePed::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	m_gameTime += DeltaTime;
	switch(m_pedState) {
		case PED_STATE_NONE:
		{
			return;
			break;
		}
		case PED_STATE_FREE:
		{
			
			break;
		}
		case PED_STATE_GROUPED:
		{
			if (!Ulib::Valid(m_squadMember)) {
				SetPedState(PED_STATE_FREE);
				break;
			}
			if (m_aicontroller) {
				float dist_to_squad = FVector::Distance(GetActorLocation(), m_squadMember->GetActorLocation());
				if (dist_to_squad <= (MOVE_TO_TOLERANCE / 2.f)) {
					FVector away_from_sq = (GetActorLocation() + m_squadMember->GetActorLocation());
					away_from_sq.Normalize();
					FVector move_to = m_squadMember->GetActorLocation() + away_from_sq * 500.f;
					m_aicontroller->MoveToLocation(move_to, 20.f);
				}
				else if(dist_to_squad > MOVE_TO_TOLERANCE)	{
					m_aicontroller->MoveToActor(m_squadMember, MOVE_TO_TOLERANCE);
				}
				if (IsCurrentTargetValid()) {
					//UE_LOG(LogTemp, Warning, TEXT("Target is valid"));
					FRotator look_at_rot = (m_target->GetActorLocation() - GetActorLocation()).Rotation();
					FRotator iteration_rot = FMath::Lerp(GetActorRotation(), look_at_rot, 0.2f);
					iteration_rot.Pitch = iteration_rot.Roll = 0.f;
					SetActorRotation(iteration_rot);
					float diff = FMath::Abs(look_at_rot.Yaw - GetActorRotation().Yaw);
					if (diff < 10.f) {
						TryShoot();
					}
				}
				else {
					FRotator lerp_rot = FMath::Lerp(GetActorRotation(), m_lookAtRot, 0.03f);
					SetActorRotation(lerp_rot);
					float diff = FMath::Abs(lerp_rot.Yaw - m_lookAtRot.Yaw);
					if (diff <= 10.f) {
						if ((m_gameTime - m_lastLookAtTime) > (m_lookWaitTolerance + FMath::FRandRange(-1.f, 2.f))) {
							//UE_LOG(LogTemp, Warning, TEXT("changed view rot"));
							m_lookAtRot.Yaw = FMath::FRandRange(-180.f, 180.f);
							m_lastLookAtTime = m_gameTime;
						}
					}
					m_target = Ulib::GetEntityInVision(this, 1.f*visionRange, 1.f*visionDeg);
				}
			}
			break;
		}
		default:
			break;
	}
}
void AevePed::SetPedState(PedState t_newState)
{
	m_pedState = t_newState;
}
bool AevePed::IsCurrentTargetValid()
{
	if (!Ulib::Valid(m_target))
		return false;

	AeveCharacter* t_character = Cast<AeveCharacter>(m_target);
	if (t_character && t_character->IsDead()) {
		m_target = nullptr;
		return false;
	}
	float dist_to_target = FVector::Distance(GetActorLocation(), m_target->GetActorLocation());
	if (dist_to_target > VALID_TARGET_TOLERANCE)
		return false;
	return true;
}
void AevePed::TryShoot()
{
	if (!m_weapPrimary)
		return;
	if (res.ammo <= 0)
		res.ammo = 300;
	if ((m_gameTime - m_lastShootTime) > 1.f / m_weapPrimary->fireRate) {
		m_weapPrimary->ShootAt(m_target);
		m_lastShootTime = m_gameTime;

	}
}