// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "character/eveNICharacter.h"
#include "sound/soundcue.h"
#include "evePlayer.generated.h"

#define WEAPON_RANGE		20000.f

class APlayerController;
class Aturret_ai_base;
class AeveNICharacter;
class UAudioComponent;
UCLASS()
class EVE_API AevePlayer : public AeveNICharacter
{
	GENERATED_BODY()
private:
	UPROPERTY()
		APlayerController*							m_controller = nullptr;

	UPROPERTY()
		Aturret_ai_base*							m_placingObj = nullptr;
	bool											m_bPlacingObjLocationValid = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Blood, meta=(AllowPrivateAccess = "true"))
	USceneComponent* bloodParticleTransformComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Blood, meta=(AllowPrivateAccess = "true"))
	UParticleSystem* bloodFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound, meta=(AllowPrivateAccess = "true"))
	float HealthPercentHeartbeatSoundThreshold = 0.3f;

	// Heartbeat sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound, meta=(AllowPrivateAccess = "true"))
	UAudioComponent* HeartbeatSoundComponent;
	
	// List of take damage sound cues
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound, meta=(AllowPrivateAccess = "true"))
	TArray<USoundCue*> TakeDamageSoundCues;

	// Death sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound, meta=(AllowPrivateAccess = "true"))
	USoundCue* DeathSoundCue;

protected:
	virtual void BeginPlay() override;
	bool HasLowHealthForHeartbeatSound() const;

public:
	AevePlayer();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class UCameraComponent* camera = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class USpringArmComponent* cameraBoom = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 playerGold = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ammoCost = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 medipackCost = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 turretCost = 40;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 allyCost = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<Aturret_ai_base> turretClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<TSubclassOf<AeveNICharacter>> allyClass;


	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void StopShooting();
	void StartShooting();
	virtual void OnDeath();
	virtual float TakeDamage(float _damage, const struct FDamageEvent& damagEvent, AController* instigator, AActor* causer);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent) void PlayerOnDeath();
	UFUNCTION(BlueprintCallable) int32 CurrentGold() { return playerGold; }
	UFUNCTION(BlueprintCallable) bool EnoughGold(int32 t_need) {
		if (playerGold >= t_need) {
			playerGold -= t_need;
			return true;
		}
		return false;
	}
	UFUNCTION(BlueprintCallable)	void SetPlacingObject(Aturret_ai_base* t_obj);
	UFUNCTION(BlueprintCallable)	bool BuyTurret();
	UFUNCTION(BlueprintCallable)	bool BuyAmmo();
	UFUNCTION(BlueprintCallable)	bool NewAlly();
	UFUNCTION(BlueprintCallable)	bool BuyMediPack();
	UFUNCTION()						void AddGold(int32 t_g) { playerGold += t_g; }
	FTimerHandle m_shootTimerHandle;
};
