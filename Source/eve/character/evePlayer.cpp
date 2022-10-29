// Fill out your copyright notice in the Description page of Project Settings.


#include "character/evePlayer.h"
#include "camera/cameracomponent.h"
#include "gameframework/springarmcomponent.h"
#include "gameframework/playercontroller.h"
#include "kismet/gameplaystatics.h"
#include "components/capsulecomponent.h"
#include "drawdebughelpers.h"
#include "lib.h"
#include "objects/weapon.h"
#include "character/turret_ai_base.h"
#include "character/evenicharacter.h"
#include "kismet/kismetsystemlibrary.h"
#include "const.h"
#include "navigationsystem.h"
#include "Components/AudioComponent.h"

AevePlayer::AevePlayer()
{
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	cameraBoom->SetupAttachment(GetRootComponent());
	camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	camera->SetupAttachment(cameraBoom);

	// Add scene component for the transform of blood particle FX
	bloodParticleTransformComponent = CreateDefaultSubobject<USceneComponent>(TEXT("BloodParticleTransform"));
	// Add to the hierarchy
	bloodParticleTransformComponent->SetupAttachment(RootComponent);

	// Add audio component to hierarchy
	HeartbeatSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("HeartBeatSound"));
}
void AevePlayer::BeginPlay()
{
	Super::BeginPlay();
	m_controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (m_controller) {
		m_controller->SetShowMouseCursor(true);
	}

}

bool AevePlayer::HasLowHealthForHeartbeatSound() const
{
	return CurrentHealthPercent() < HealthPercentHeartbeatSoundThreshold;
}

// Called every frame
void AevePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (m_controller) {
		FVector screen_loc, screen_dir;
		m_controller->DeprojectMousePositionToWorld(screen_loc, screen_dir);
		FHitResult hit;
		FVector trace_start = screen_loc;
		FVector trace_end = trace_start + screen_dir * 200000.f;
		GetWorld()->LineTraceSingleByChannel(hit, trace_start, trace_end, ECC_Camera);
		if (hit.bBlockingHit) {
			FVector hit_loc = hit.Location;
			FRotator look_at_rot = (hit_loc - GetActorLocation()).Rotation();
			FRotator lerp_rot = FMath::Lerp(GetActorRotation(), look_at_rot, 1.f);
			lerp_rot.Roll = 0;
			lerp_rot.Pitch = 0;
			//SetActorRotation(lerp_rot);
			m_controller->SetControlRotation(lerp_rot);
		}
		bool b_ok = 0;
		if (Ulib::Valid(m_placingObj)) {
			GetWorld()->LineTraceSingleByChannel(hit, trace_start, trace_end, ECC_GameTraceChannel1);
			if (hit.bBlockingHit) {
				m_bPlacingObjLocationValid = 1;
				FVector t_set_loc = hit.Location;
				t_set_loc.Z += 0.5f * m_placingObj->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

				TArray<AActor*> t_actors;
				TArray<TEnumAsByte<EObjectTypeQuery>> object_types;
				object_types.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
				TArray<AActor*> IgnoredActors;
				IgnoredActors.Add(m_placingObj);
				UKismetSystemLibrary::SphereOverlapActors(GetWorld(), t_set_loc, 50.f, object_types, Aturret_ai_base::StaticClass(), IgnoredActors, t_actors);
				bool b_overlap = 0;
				for (AActor* actor : t_actors) {
					if (Ulib::Valid(actor)) {
						b_overlap = 1;
						break;
					}
				}
				if (!b_overlap) {
					m_placingObj->SetActorLocation(t_set_loc);
					b_ok = 1;
				}
				
			}
		}
		m_bPlacingObjLocationValid = b_ok;

		// Are we at low health? Then play the heartbeat sound cue
		if (HasLowHealthForHeartbeatSound())
		{
			// If we are not already playing the heartbeat sound, play it
			if (!HeartbeatSoundComponent->IsPlaying())
			{
				HeartbeatSoundComponent->Play();
			}
		}
		else
		{
			// If we are playing the heartbeat sound, stop it
			if (HeartbeatSoundComponent->IsPlaying())
			{
				HeartbeatSoundComponent->Stop();
			}
		}
	}
}

// Called to bind functionality to input
void AevePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AevePlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AevePlayer::MoveRight);
	//PlayerInputComponent->BindAxis("Turn", this, &AevePlayer::AddControllerYawInput);
	//PlayerInputComponent->BindAxis("LookUp", this, &AevePlayer::AddControllerPitchInput);

	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AevePlayer::StartShooting);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AevePlayer::StopShooting);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AevePlayer::Reload);

}

void AevePlayer::StartShooting()
{
	if (Ulib::Valid(m_placingObj)) {
		if (m_bPlacingObjLocationValid) {
			m_placingObj->SetActive();
			m_placingObj = nullptr;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Invalid Turret Loc"));
			Ulib::Destroy(m_placingObj);
		}
		return;
	}
	if (!m_weapPrimary)
		return;
	GetWorld()->GetTimerManager().SetTimer(m_shootTimerHandle, this, &AevePlayer::Shoot, 1 / m_weapPrimary->fireRate, true, 0.f);

}

void AevePlayer::StopShooting()
{
	GetWorld()->GetTimerManager().ClearTimer(m_shootTimerHandle);
}
void AevePlayer::OnDeath()
{
	Super::OnDeath();
	PlayerOnDeath();
	
	// Play death sound cue
	UGameplayStatics::PlaySound2D(this, DeathSoundCue);
	
	//Ulib::Destroy(this);
}
void AevePlayer::SetPlacingObject(Aturret_ai_base* t_obj)
{
	if (!Ulib::Valid(t_obj))
		return;
	if (Ulib::Valid(m_placingObj))
		UE_LOG(LogTemp, Warning, TEXT("Already placing a turret"));
	m_placingObj = t_obj;
}
bool AevePlayer::BuyTurret()
{
	if (!turretClass)
		return false;
	if (!EnoughGold(turretCost))
		return false;
	FActorSpawnParameters t_spawnParams;
	t_spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Aturret_ai_base* t_turret = GetWorld()->SpawnActor<Aturret_ai_base>(turretClass, FVector(0.f, 0.f, 9990.f), FRotator::ZeroRotator, t_spawnParams);
	if (Ulib::Valid(t_turret)) {
		SetPlacingObject(t_turret);
		return true;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("eve player turret null spawn"));
		Ulib::Destroy(t_turret);
	}
	return false;
}
bool AevePlayer::BuyAmmo()
{
	if (EnoughGold(ammoCost)) {
		res.ammo += 200;
	}
	else {
		return false;
	}
	return true;
}
bool AevePlayer::NewAlly()
{
	if (allyClass.Num() == 0)
		return false;
	if (!EnoughGold(allyCost)) {
		return false;
	}
	FActorSpawnParameters t_spawnParams;
	t_spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	FNavLocation t_nav;
	UNavigationSystemV1* c_nav = UNavigationSystemV1::GetCurrent(this);
	if (!c_nav) {
		UE_LOG(LogTemp, Warning, TEXT("null nav system"));
		playerGold += allyCost;
		return false;
	}
	if (c_nav->GetRandomReachablePointInRadius(GetActorLocation(), 2500.f, t_nav) == false) {
		playerGold += allyCost;
		UE_LOG(LogTemp, Warning, TEXT("no ally spawn point"));
		return false;
	}
	FVector t_spawnLoc = t_nav.Location;
	AeveNICharacter* t_ally = GetWorld()->SpawnActor<AeveNICharacter>(allyClass[FMath::RandRange(0, allyClass.Num() - 1)], t_spawnLoc, FRotator::ZeroRotator, t_spawnParams);
	if (Ulib::Valid(t_ally)) {
		UE_LOG(LogTemp, Warning, TEXT("ally spawned"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("eve player ally null spawn"));
		return false;
	}
	return true;
}
bool AevePlayer::BuyMediPack()
{
	if (!EnoughGold(medipackCost))
		return false;
	health = 100.f;
	return true;
}

float AevePlayer::TakeDamage(float _damage, const struct FDamageEvent& damageEvent, AController* controller, AActor* causer)
{
	_damage = Super::TakeDamage(_damage, damageEvent, controller, causer);

	// Show blood particle FX using Cascade
	if (bloodFX) {
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			bloodFX,
			bloodParticleTransformComponent->GetComponentLocation(),
			bloodParticleTransformComponent->GetComponentRotation(),
			bloodParticleTransformComponent->GetComponentScale()
		);
	}

	// Play a random take damage sound cue
	if (TakeDamageSoundCues.Num() > 0) {
		UGameplayStatics::PlaySound2D(this, TakeDamageSoundCues[FMath::RandRange(0, TakeDamageSoundCues.Num() - 1)]);
	}
	
	return _damage;
}