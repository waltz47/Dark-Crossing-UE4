// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "gameframework/character.h"
#include "lib.generated.h"

#define ni_character_tag			"ni_world_character"
#define i_character_tag				"i_world_character"
UCLASS()
class EVE_API Ulib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static bool Valid(AActor* actor);
	static void Destroy(AActor* actor);
	static void	SetActorNI(AActor* actor);
	static void	SetActorI(AActor* actor);
	static bool IsInfected(AActor* actor) {
		return Ulib::Valid(actor) && actor->ActorHasTag(i_character_tag);
	}
	static bool IsHostile(AActor* self, AActor* other) {
		if (!Ulib::Valid(self) || !Ulib::Valid(other))
			return false;
		return IsInfected(self) ^ IsInfected(other);
	}
	static ACharacter* GetEntityInVision(AActor* actor, float cone_radius, float deg);
	static TArray<ACharacter*> GetAllNI(UObject* contextObj);
};
