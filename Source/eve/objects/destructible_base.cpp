// Fill out your copyright notice in the Description page of Project Settings.


#include "objects/destructible_base.h"
#include "components/staticmeshcomponent.h"
#include "components/capsulecomponent.h"
#include "kismet/gameplaystatics.h"
#include "character/eveinfected.h"
#include "lib.h"
#include "particles/particlesystemcomponent.h"
#include "const.h"

// Sets default values
Adestructible_base::Adestructible_base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	objCollider = CreateDefaultSubobject<UCapsuleComponent>("Collider Component");
	SetRootComponent(objCollider);
	objCollider->OnComponentBeginOverlap.AddDynamic(this, &Adestructible_base::OnOverlapBegin);
	objMesh = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh");
	objMesh->SetupAttachment(objCollider);
}

// Called when the game starts or when spawned
void Adestructible_base::BeginPlay()
{
	Super::BeginPlay();
	
}

float Adestructible_base::TakeDamage(float damage, const struct FDamageEvent& dmg_event, AController* _instigator, AActor* causer)
{
	damage = Super::TakeDamage(damage, dmg_event, _instigator, causer);
	if (m_over) 
		return 0.f;
	UE_LOG(LogTemp, Warning, TEXT("took damage"));
	m_damageTaken += damage;
	if (m_damageTaken >= damageToDestroy) {
		Explode();
		m_over = 1;
	}
	return damage;
}
void Adestructible_base::Explode()
{
	if (explosionEmitter) {
		t_spawned_particle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionEmitter, GetActorLocation(), GetActorRotation());
	}
	GetWorld()->GetTimerManager().SetTimer(m_overTimerHandle, this, &Adestructible_base::DestroyObject, lifeTime, false);
	objMesh->DestroyComponent();
	objCollider->SetCapsuleHalfHeight(100.f);
	objCollider->SetCapsuleRadius(100.f);
}
void Adestructible_base::DestroyObject()
{
	if (t_spawned_particle) {
		t_spawned_particle->DestroyComponent();
	}
	Ulib::Destroy(this);
}

void Adestructible_base::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!m_over)
		return;
	AeveInfected* t_other = Cast<AeveInfected>(OtherActor);
	if (Ulib::Valid(t_other) == false)
		return;
	UE_LOG(LogTemp, Warning, TEXT("status_onfire"));
	t_other->AddStatusEffect(STATUS_ONFIRE, statusEffectEmitter);
}