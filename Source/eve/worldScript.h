// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "const.h"
#include "worldScript.generated.h"

class Aaidirector;
UCLASS()
class EVE_API AworldScript : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AworldScript();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class USphereComponent* collisionSphere = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<Aaidirector*> activateActors;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void ActivateFunc();
};
