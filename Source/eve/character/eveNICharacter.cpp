// Fill out your copyright notice in the Description page of Project Settings.


#include "character/eveNICharacter.h"
#include "const.h"
#include "components/spotlightcomponent.h"
#include "lib.h"
#include "objects/weapon.h"

AeveNICharacter::AeveNICharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	flashLight = CreateDefaultSubobject<USpotLightComponent>("Flash Light Comp");
	flashLight->SetupAttachment(GetRootComponent());
}
void AeveNICharacter::BeginPlay()
{
	Super::BeginPlay();
	Ulib::SetActorNI(this);
	if (weaponClass)
		m_weapPrimary = Cast<Aweapon>(GetWorld()->SpawnActor<Aweapon>(weaponClass, GetMesh()->GetSocketLocation(WEAPON_ATTACH_SOCKET), GetActorForwardVector().Rotation()));
	if (m_weapPrimary) {
		m_weapPrimary->m_owner = this;
		m_weapPrimary->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WEAPON_ATTACH_SOCKET);
		//m_weapPrimary->SetActorLocation(GetMesh()->GetComponentLocation());
	}

	
}
void AeveNICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AeveNICharacter::Shoot()
{
	if (!Ulib::Valid(m_weapPrimary))
		return;
	m_weapPrimary->Shoot();
}
void AeveNICharacter::ShootAt(AActor* actor)
{
	if (!Ulib::Valid(actor))
		return;
	if (!Ulib::Valid(m_weapPrimary))
		return;
	m_weapPrimary->ShootAt(actor);

}
void AeveNICharacter::Reload()
{
	if (!Ulib::Valid(m_weapPrimary))
		return;
	m_weapPrimary->Reload();
}
void AeveNICharacter::OnDeath()
{
	Super::OnDeath();
	Ulib::Destroy(m_weapPrimary);
	
}

void AeveNICharacter::FlashLightMode(bool t_flashLight)
{
	flashLight->SetHiddenInGame(!t_flashLight);
}

int32 AeveNICharacter::GetClipAmmo()
{
	if (Ulib::Valid(m_weapPrimary))
		return m_weapPrimary->m_clipAmmo;
	return 0;
}
int32 AeveNICharacter::GetClipSize()
{
	if (Ulib::Valid(m_weapPrimary))
		return m_weapPrimary->clipSize;
	return 0;
}