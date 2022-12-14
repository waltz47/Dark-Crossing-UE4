// Fill out your copyright notice in the Description page of Project Settings.


#include "objects/weapon.h"
#include "components/scenecomponent.h"
#include "components/skeletalmeshcomponent.h"
#include "lib.h"
#include "character/evecharacter.h"
#include "const.h"
#include "kismet/gameplaystatics.h"
#include "kismet/kismetsystemlibrary.h"
#include "sound/soundcue.h"
// Sets default values
Aweapon::Aweapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	rootScene = CreateDefaultSubobject<USceneComponent>("Root Scene Comp");
	SetRootComponent(rootScene);
	gunMesh = CreateDefaultSubobject<USkeletalMeshComponent>("GunMesh");
	gunMesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void Aweapon::BeginPlay()
{
	Super::BeginPlay();
	m_clipAmmo = clipSize;
	if (!Ulib::Valid(m_owner))
		return;
	
}
void Aweapon::ShootAt(AActor* actor)
{
	if (!Ulib::Valid(actor))
		return;
	if (IsReloading())
		return;
	if (!Ulib::Valid(m_owner))
		return;
	if (m_clipAmmo == 0)
	{
		Reload();
		return;
	}
	m_clipAmmo--;
	MuzzleEffect();
	MuzzleSound();
	if ((FMath::RandRange(0, 100) % 2) == 0) {
		UGameplayStatics::ApplyDamage(actor, damage, m_owner->GetController(), m_owner, UDamageType::StaticClass());
		FHitResult hit;
		GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), m_owner->GetActorLocation() + m_owner->GetActorForwardVector() * GUN_RANGE, ECC_Camera);
		if (characterImpactFX && hit.bBlockingHit) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), characterImpactFX, hit.Location, hit.ImpactNormal.Rotation());
		}
	}
}
EShotResult Aweapon::Shoot()
{
	// Can't shoot without ammo!
	if (OutOfAmmo())
	{
		return EShotResult::NoAmmo;
	}
	
	if (IsReloading()) 
		return EShotResult::AlreadyReloading;
	if (!Ulib::Valid(m_owner))
		return EShotResult::Error;
	if (m_clipAmmo == 0)
	{
		Reload();
		return EShotResult::StartedReloading;
	}
	m_clipAmmo--;
	FVector trace_start = GetActorLocation() + m_owner->GetActorForwardVector() * 10.f;
	FVector trace_end = trace_start + m_owner->GetActorForwardVector() * GUN_RANGE;
	TArray<FHitResult> out_hits;
	float t_damage = (damage + m_owner->damage) / 1.5f;

	TArray<TEnumAsByte<EObjectTypeQuery>> object_types;
	TArray<AActor*> ignored_actors;

	ignored_actors.Add(m_owner);

	//object_types.Add(UEngineTypes::ConvertToObjectType(ECC_Camera));
	//object_types.Add(UEngineTypes::ConvertToObjectType(ECC_Visibility));
	object_types.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	object_types.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	object_types.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	UKismetSystemLibrary::LineTraceMultiForObjects(this, trace_start, trace_end, object_types, false, ignored_actors, EDrawDebugTrace::None, out_hits, true);
	for (FHitResult hit : out_hits) {
		//if(hit.GetActor()) UE_LOG(LogTemp, Warning, TEXT("hit %s"), *hit.GetActor()->GetName());
		if (!hit.bBlockingHit || !Ulib::Valid(hit.GetActor()))
			continue;
		if (!(hit.GetActor()->ActorHasTag(ni_character_tag))) {
			UGameplayStatics::ApplyDamage(hit.GetActor(), t_damage, m_owner->GetController(), m_owner, UDamageType::StaticClass());
			t_damage *= 0.8f;
			if (characterImpactFX) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), characterImpactFX, hit.Location, hit.ImpactNormal.Rotation());
			}
		}
		else if (!(hit.GetActor()->ActorHasTag(ni_character_tag))) {
			//imp
			if (otherImpactFX) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), otherImpactFX, hit.Location, hit.ImpactNormal.Rotation());
			}
		}
	}
	
	//spawn particle effects
	MuzzleEffect();
	MuzzleSound();
	
	return EShotResult::Shot;
}
void Aweapon::Reload()
{
	if (IsReloading())
		return;
	StartReload();
}
void Aweapon::StartReload()
{
	if (!Ulib::Valid(m_owner) || m_owner->res.ammo == 0)
		return;
	if (IsReloading() || m_clipAmmo == clipSize)
		return;
	if (gunReloadSFX) {
		UGameplayStatics::PlaySoundAtLocation(this, gunReloadSFX, GetActorLocation(), GetActorRotation());
	}
	GetWorld()->GetTimerManager().SetTimer(reloadTimer, this, &Aweapon::StopReload, m_reloadTime, false);
}
void Aweapon::StopReload()
{
	if (!Ulib::Valid(m_owner))
		return;
	const int32 totalAmmo = GetTotalAmmo();
	m_clipAmmo = 0;
	const int32 fill = FMath::Min(totalAmmo, clipSize);
	m_clipAmmo = fill;
	m_owner->res.ammo = totalAmmo - fill;
}
bool Aweapon::IsReloading()
{
	return GetWorld()->GetTimerManager().IsTimerActive(reloadTimer);
}
void Aweapon::MuzzleEffect()
{
	if (muzzleFlash) {
		FVector location = gunMesh->GetSocketLocation(MUZZLE_NAME);
		FRotator rotation = gunMesh->GetSocketRotation(MUZZLE_NAME);
		UGameplayStatics::SpawnEmitterAttached(muzzleFlash, gunMesh, MUZZLE_NAME, location, rotation, EAttachLocation::KeepWorldPosition);
	}
}
void Aweapon::MuzzleSound()
{
	// Play shoot sound
	if (MuzzleSoundCues.Num() > 0)
	{
		const auto MuzzleSoundCue = MuzzleSoundCues[FMath::RandRange(0, MuzzleSoundCues.Num() - 1)];
		if (IsValid(MuzzleSoundCue))
		{
			// Make concurrency sound settings for ths sound cue that is limited to owner
			UGameplayStatics::PlaySoundAtLocation(this, MuzzleSoundCue, GetActorLocation(), MuzzleSoundMultiplier, 1, 0, nullptr, MuzzleSoundConcurrency);
		}
	}
}

int Aweapon::GetTotalAmmo() const
{
	return m_owner->res.ammo + m_clipAmmo;
}

bool Aweapon::OutOfAmmo() const
{
	return GetTotalAmmo() <= 0;
}
