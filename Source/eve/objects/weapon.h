// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "weapon.generated.h"

enum class EShotResult
{
	Shot,
	NoAmmo,
	AlreadyReloading,
	StartedReloading,
	Error
};

class AeveCharacter;
#define MUZZLE_NAME		FName("weapon_muzzle")
#define GUN_RANGE		50000.f
UCLASS()
class EVE_API Aweapon : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class USceneComponent* rootScene = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class USkeletalMeshComponent* gunMesh = nullptr;

	UPROPERTY() AeveCharacter*										m_owner = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float				damage = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32				clipSize = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float				fireRate = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UParticleSystem*	muzzleFlash = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) class USoundCue*	fireSound = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USoundCue*			gunReloadSFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UParticleSystem*	characterImpactFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UParticleSystem*	otherImpactFX = nullptr;
	int32															m_clipAmmo = 0;
	float															m_reloadTime = 1.5f;

	// List of muzzle sound cues
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound, meta=(AllowPrivateAccess = "true"))
	TArray<USoundCue*> MuzzleSoundCues;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MuzzleSoundMultiplier = 1.0f;

	// Sound concurrency settings for the muzzle sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound, meta=(AllowPrivateAccess = "true"))
	USoundConcurrency* MuzzleSoundConcurrency;

	

	Aweapon();
	virtual void ShootAt(AActor* actor);
	virtual EShotResult Shoot();
	virtual void StartReload();
	virtual void StopReload();
	virtual void Reload();
	virtual bool IsReloading();
	void MuzzleEffect();
	void MuzzleSound();
	int GetTotalAmmo() const;
	bool OutOfAmmo() const;
	

	FTimerHandle reloadTimer;
	friend class AeveCharacter;
};

