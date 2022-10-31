// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "character/eveCharacter.h"
#include "eveNICharacter.generated.h"

class USpotLightComponent;
class Aweapon;
#define WEAPON_ATTACH_SOCKET		FName("weapon_socket")
UCLASS()
class EVE_API AeveNICharacter : public AeveCharacter
{
	GENERATED_BODY()
	
private:
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY() Aweapon* m_weapPrimary = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USpotLightComponent* flashLight = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<Aweapon> weaponClass;



	AeveNICharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void Shoot();
	virtual void Reload();
	virtual void ShootAt(AActor* actor);
	virtual void OnDeath() override;
	void			FlashLightMode(bool t_flashLight);
	UFUNCTION(BlueprintCallable)
		int32 GetClipAmmo();

	UFUNCTION(BlueprintCallable)
		int32 GetClipSize();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UStaticMeshComponent* FriendlyCharacterMarkerMesh;
};
