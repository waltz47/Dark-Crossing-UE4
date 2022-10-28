// Fill out your copyright notice in the Description page of Project Settings.


#include "character/turret_ai_base.h"
#include "const.h"
#include "lib.h"
#include "kismet/gameplaystatics.h"
#include "character/evecharacter.h"
#include "gameframework/charactermovementcomponent.h"
#include "kismet/kismetsystemlibrary.h"
#include "drawdebughelpers.h"

Aturret_ai_base::Aturret_ai_base()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
	if (GetCharacterMovement()) {
		GetCharacterMovement()->DisableMovement();
	}
	turretBase = CreateDefaultSubobject<UStaticMeshComponent>("Turret Base Mesh");
	turretBase->SetupAttachment(GetRootComponent());
	turretBarrel = CreateDefaultSubobject<UStaticMeshComponent>("Turret barrel Mesh");
	turretBarrel->SetupAttachment(turretBase);


}
void Aturret_ai_base::BeginPlay()
{
	Super::BeginPlay();

}
void Aturret_ai_base::SetActive()
{
	Ulib::SetActorNI(this);
	FTimerHandle t_search_th;
	GetWorld()->GetTimerManager().SetTimer(t_search_th, this, &Aturret_ai_base::Search, SEARCH_TIME, true);
	SetActorTickEnabled(true);
}

void Aturret_ai_base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	m_gameTime += DeltaTime;
	if (ValidTarget()) {
		FRotator t_look_at_rot = (m_target->GetActorLocation() - GetActorLocation()).Rotation();
		t_look_at_rot.Yaw -= 90.f;
		t_look_at_rot.Pitch = 0.f;
		float diff = FMath::Abs(turretBarrel->GetComponentRotation().Yaw - t_look_at_rot.Yaw);
		if (diff <= 10.f) {
			Shoot();
		}
		else {
			FRotator t_temp_rot = FMath::Lerp(turretBarrel->GetComponentRotation(), t_look_at_rot, 0.3f);
			t_temp_rot.Pitch = 0.f;
			turretBarrel->SetWorldRotation(t_temp_rot);
		}
	}
	else {
		
	}
}

void Aturret_ai_base::OnDeath()
{
	Ulib::Destroy(this);
}
void Aturret_ai_base::Shoot()
{
	if ((m_gameTime - m_lastShotAt) < (1.f / fireRate))
		return;
	m_lastShotAt = m_gameTime;
	//vfx + sfx 
	if (ValidTarget()) {
		DrawDebugLine(GetWorld(), GetActorLocation(), m_target->GetActorLocation(), FColor::Red, false, -1.f, 0, 1.f);
		UGameplayStatics::ApplyDamage(m_target, damage, nullptr, this, UDamageType::StaticClass());
	}
}
bool Aturret_ai_base::ValidTarget()
{
	if (!Ulib::Valid(m_target))
		return false;
	if (m_target->IsDead())
		return false;
	float t_dst = FVector::Distance(m_target->GetActorLocation(), GetActorLocation());
	return t_dst <= attackRange;
}
void Aturret_ai_base::Search()
{
	if (!ValidTarget()) {
		TArray<AActor*> t_actors;
		TArray<TEnumAsByte<EObjectTypeQuery>> object_types;
		object_types.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), attackRange, object_types, ACharacter::StaticClass(), IgnoredActors, t_actors);
		for (AActor* actor : t_actors) {
			AeveCharacter* t_c = Cast<AeveCharacter>(actor);
			if (!Ulib::Valid(t_c))
				continue;
			if (t_c->IsDead())
				continue;
			if (t_c->ActorHasTag(i_character_tag) == false)
				continue;
			m_target = t_c;
			break;
		}
	}
}