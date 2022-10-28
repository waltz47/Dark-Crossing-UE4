// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "destructible_base.generated.h"

UCLASS()
class EVE_API Adestructible_base : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Adestructible_base();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class UStaticMeshComponent* objMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class UCapsuleComponent* objCollider = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UParticleSystem* explosionEmitter = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UParticleSystem* statusEffectEmitter = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float damageToDestroy = 40.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float lifeTime = 10.f;

	FTimerHandle m_overTimerHandle;
	float m_damageTaken = 0.f;
	bool m_over = 0;
	UPROPERTY()
		UParticleSystemComponent* t_spawned_particle = nullptr;

	UFUNCTION()
		virtual float TakeDamage(float damage, const struct FDamageEvent& dmg_event, AController* _instigator, AActor* causer);


	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void Explode();
	virtual void DestroyObject();

};
