// Fill out your copyright notice in the Description page of Project Settings.


#include "character/eveCharacter.h"
#include "lib.h"
#include "components/capsulecomponent.h"
#include "gameframework/charactermovementcomponent.h"

// Sets default values
AeveCharacter::AeveCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AeveCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Remember the max health for future calculations
	maxHealth = health;
}

float AeveCharacter::CurrentHealthPercent() const
{
	return health / maxHealth;
}

// Called every frame
void AeveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AeveCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
void AeveCharacter::MoveForward(float val)
{
	AddMovementInput(FVector(1.f, 1.f, 0.f), val);
}

void AeveCharacter::MoveRight(float val)
{
	AddMovementInput(FVector(-1.f, 1.f, 0.f), val);
}
void AeveCharacter::OnDeath()
{
	Tags.Empty();
	if(GetWorld())
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	DetachFromControllerPendingDestroy();
	SetActorEnableCollision(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionProfileName(FName("NoCollision"));
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//Ulib::Destroy(this);
	SetActorTickEnabled(false);
	GetCharacterMovement()->DisableMovement();
	GetWorld()->GetTimerManager().SetTimer(deathTimer, this, &AeveCharacter::DestroyObject, 100.f, true);
}
float AeveCharacter::TakeDamage(float dmg, const struct FDamageEvent& eevent, AController* causer, AActor* actor)
{
	dmg = Super::TakeDamage(dmg, eevent, causer, actor);
	if (IsDead())
		return 0.f;
	if (armor > 0.f) {
		dmg = dmg * (dmg / (dmg + armor));
	}
	health -= dmg;
	if (health <= 0.f) {
		OnDeath();
	}	
	return dmg;
}
void AeveCharacter::DestroyObject()
{
	Ulib::Destroy(this);
}
bool AeveCharacter::IsDead()
{
	return GetWorld()->GetTimerManager().IsTimerActive(deathTimer);
}

