// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "pickup.generated.h"

struct FResource;
enum class EResourceEnum:uint8;
//class UTextRenderComponent;
//class AevePlayer;
UCLASS()
class EVE_API Apickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Apickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class USceneComponent* rootScene = nullptr;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UTextRenderComponent* displayText = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float pickupRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float quantity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EResourceEnum resource;

	bool m_bTextActive = 0;
	virtual void DisplayText();
	virtual void HideText();
	virtual void Use();
	FText GetResourceName();



};
