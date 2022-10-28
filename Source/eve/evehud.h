// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "evehud.generated.h"

/**
 * 
 */
UCLASS()
class EVE_API Aevehud : public AHUD
{
	GENERATED_BODY()
public:
	Aevehud();

	virtual void DrawHUD() override;

	
};
