// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "const.generated.h"

//UENUM()
//enum ObjectState
//{
//	STATE_NONE,
//	STATE_DAMAGED,
//	STATE_MOVED,
//	STATE_INTERACTED,
//	STATE_DESTROYED
//};
//
//DECLARE_MULTICAST_DELEGATE_TwoParams(FQuestData, ObjectState, FString);

UENUM(BlueprintType)
enum InfectedState
{
	INFECTED_STATE_NONE,
	INFECTED_STATE_IDLE,
	INFECTED_STATE_ROAM,
	INFECTED_STATE_CHASE
};

UENUM(BlueprintType)
enum PedState
{
	PED_STATE_NONE,
	PED_STATE_FREE,
	PED_STATE_GROUPED
};

UENUM(BlueprintType)
enum class EResourceEnum : uint8 {
	RESOURCE_NONE,
	RESOURCE_AMMO,
	RESOURCE_MEDIPACK
};

UENUM()
enum StatusEffect
{
	STATUS_NONE = 1<<0,
	STATUS_ONFIRE = 1<<1
};

USTRUCT(BlueprintType)
struct FResource
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ammo = 200;
};

