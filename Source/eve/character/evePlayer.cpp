// Fill out your copyright notice in the Description page of Project Settings.


#include "character/evePlayer.h"
#include "camera/cameracomponent.h"
#include "gameframework/springarmcomponent.h"
#include "gameframework/playercontroller.h"
#include "kismet/gameplaystatics.h"
#include "drawdebughelpers.h"
#include "lib.h"
#include "objects/weapon.h"
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