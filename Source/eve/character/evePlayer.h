// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "character/eveNICharacter.h"
#include "evePlayer.generated.h"

#define WEAPON_RANGE		20000.f

class APlayerController;
class Aturret_ai_base;
class AeveNICharacter;
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

protected:
	virtual void BeginPlay() override;

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
	UFUNCTION(BlueprintCallable) int32 CurrentGold() { return playerGold; }
	UFUNCTION(BlueprintCallable) bool EnoughGold(int32 t_need) {
		if (playerGold >= t_need) {
			playerGold -= t_need;
			return true;
		}
		return false;
	}
	UFUNCTION(BlueprintCallable) void SetPlacingObject(Aturret_ai_base* t_obj);
	UFUNCTION(BlueprintCallable) bool BuyTurret();
	UFUNCTION(BlueprintCallable) bool BuyAmmo();
	UFUNCTION(BlueprintCallable) bool NewAlly();
	UFUNCTION(BlueprintCallable) bool BuyMediPack();

	FTimerHandle m_shootTimerHandle;
};
