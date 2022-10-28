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
#include "const.h"

AevePlayer::AevePlayer()
{
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	cameraBoom->SetupAttachment(GetRootComponent());
	camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	camera->SetupAttachment(cameraBoom);
}
void AevePlayer::BeginPlay()
{
	Super::BeginPlay();
	m_controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (m_controller) {
		m_controller->SetShowMouseCursor(true);
	}

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
		if (Ulib::Valid(m_placingObj)) {
			GetWorld()->LineTraceSingleByChannel(hit, trace_start, trace_end, ECC_GameTraceChannel1);
			if (hit.bBlockingHit) {
				m_bPlacingObjLocationValid = 1;
				FVector t_set_loc = hit.Location;
				t_set_loc.Z += 0.5f * m_placingObj->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
				m_placingObj->SetActorLocation(t_set_loc);
			} else 	m_bPlacingObjLocationValid = 0;
		} else 	m_bPlacingObjLocationValid = 0;
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