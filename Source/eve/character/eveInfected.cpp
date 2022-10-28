// Fill out your copyright notice in the Description page of Project Settings.


#include "character/eveInfected.h"
#include "lib.h"
#include "const.h"
#include "kismet/gameplaystatics.h"
#include "aicontroller.h"
#include "kismet/kismetmathlibrary.h"
#include "gameframework/charactermovementcomponent.h"
#include "navigationsystem.h"
#include "character/aidirector.h"
#include "gameframework/actor.h"
#include "character/eveCharacter.h"
#include "character/eveplayer.h"
#include "particles/particlesystemcomponent.h"

AeveInfected::AeveInfected()
{
	PrimaryActorTick.bCanEverTick = true;
	SetInfectedState(INFECTED_STATE_ROAM);
}

void AeveInfected::BeginPlay()
{
	Super::BeginPlay();
	Ulib::SetActorI(this);
	m_aicontroller = Cast<AAIController>(GetController());
	
}
void AeveInfected::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	m_gameTime += DeltaTime;
	if (m_statusEffect == STATUS_ONFIRE && (m_gameTime - m_lastStatusDamageSnap) > statusDamageTick) {
		UGameplayStatics::ApplyDamage(this, fireDamage, nullptr, nullptr, UDamageType::StaticClass());
		m_lastStatusDamageSnap = m_gameTime;
	}
	switch (m_infectedState) {
		case INFECTED_STATE_NONE:
		{
			return;
			break;
		}
		case INFECTED_STATE_ROAM:
		{
			m_target = Ulib::GetEntityInVision(this, visionRange, visionDeg);
			if (Ulib::Valid(m_target)) {
				SetInfectedState(INFECTED_STATE_CHASE);
				break;
			}
			if (GetVelocity().Size() > 0.f) {
				break;
			}
			for (int32 i = 0; i < NUM_RANDOM_POINTS; i++) {
				float dx = i * 1000.f;
				float temp_factor = FMath::FRandRange(-2000, 2000.f);
				FVector loc = GetActorLocation() + GetActorForwardVector() * dx + GetActorRightVector() * temp_factor;
				if ((FMath::RandRange(1, 100) % 2) && UNavigationSystemV1::FindPathToLocationSynchronously(this, GetActorLocation(), loc, this)) {
					m_aicontroller->MoveToLocation(loc);
				}
			}
			//eqs
			break;
		}
		case INFECTED_STATE_CHASE:
		{
			if (!Ulib::Valid(m_target)) {
				SetInfectedState(INFECTED_STATE_ROAM);
				break;			
			}
			AeveCharacter* character = Cast<AeveCharacter>(m_target);
			if (character && character->IsDead()) {
				m_target = nullptr;
				SetInfectedState(INFECTED_STATE_ROAM);
				break;
			}
			float dist_to_target = FVector::Distance(m_target->GetActorLocation(), GetActorLocation());
			if (dist_to_target > MELEE_RANGE) {
				if (m_aicontroller) {
					m_aicontroller->MoveToActor(m_target);
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("No controller found for infected"));
				}
				break;
			}
			FRotator look_at_rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), m_target->GetActorLocation());
			float diff = FMath::Abs(look_at_rot.Yaw - GetActorRotation().Yaw);
			if (diff > LOOK_AT_TOLERANCE) {
				FRotator set_rot = FMath::Lerp(GetActorRotation(), look_at_rot, 0.2f);
				set_rot.Pitch = set_rot.Roll = 0.f;
				SetActorRotation(set_rot);
			}
			else {
				MeleeAttack();
			}
			break;
		}
	}
}
void AeveInfected::SetInfectedState(InfectedState t_newState) {
	m_infectedState = t_newState;
	switch (m_infectedState) {
		case INFECTED_STATE_ROAM:
		{
			if (GetCharacterMovement())
				GetCharacterMovement()->MaxWalkSpeed = roamSpeed;
			break;
		}
		case INFECTED_STATE_CHASE:
		{
			if (GetCharacterMovement())
				GetCharacterMovement()->MaxWalkSpeed = chaseSpeed;
			break;
		}
		default:
			break;
	}
}
void AeveInfected::AddStatusEffect(StatusEffect t_add_effect, UParticleSystem* t_ps)
{
	if (m_statusEffect == t_add_effect || (!t_ps))
		return;
	m_statusEffect = t_add_effect;
	switch (m_statusEffect) {
		case STATUS_NONE:
			break;
		case STATUS_ONFIRE:
		{
			FName socketName = FName("Spine1");
			FVector loc = GetMesh()->GetSocketLocation(socketName);
			m_status_fire_particle = UGameplayStatics::SpawnEmitterAttached(t_ps, GetMesh(), socketName, loc, FRotator::ZeroRotator, FVector(0.8f, 0.8f, 0.8f), EAttachLocation::KeepWorldPosition);
			break;
		}
	}
}
void AeveInfected::MeleeAttack()
{
	if (m_bAttacking || !Ulib::Valid(m_target))
		return;
	m_bAttacking = 1;
}
void AeveInfected::Attack_Over()
{
	m_bAttacking = 0;
	if (Ulib::Valid(m_target) && FVector::Distance(GetActorLocation(), m_target->GetActorLocation()) <= 1.5f * MELEE_RANGE) {
		UGameplayStatics::ApplyDamage(m_target, damage, m_aicontroller, this, UDamageType::StaticClass());
	}
}
float AeveInfected::TakeDamage(float _damage, const struct FDamageEvent& damageEvent, AController* controller, AActor* causer)
{
	_damage = Super::TakeDamage(_damage, damageEvent, controller, causer);
	if (IsDead()) {
		if (m_bDeadFlag)
			return 0.f;
		m_bDeadFlag = 1;
	}
	if (Ulib::Valid(Cast<ACharacter>(causer)) && Ulib::IsHostile(this, causer)) {
		if (!Ulib::Valid(m_target)) {
			m_target = Cast<ACharacter>(causer);
			SetInfectedState(INFECTED_STATE_CHASE);
		}
	}
	return _damage;
}
void AeveInfected::OnDeath()
{
	if (m_status_fire_particle) {
		m_status_fire_particle->DestroyComponent();
	}
	Super::OnDeath();
}