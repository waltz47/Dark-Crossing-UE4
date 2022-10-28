// Fill out your copyright notice in the Description page of Project Settings.


#include "lib.h"
#include "kismet/gameplaystatics.h"
#include "kismet/kismetsystemlibrary.h"
#include "kismet/kismetmathlibrary.h"
#include "character/evenicharacter.h"
#include "character/evecharacter.h"

bool Ulib::Valid(AActor* actor)
{
	if (!actor)
		return false;
	if (!actor->IsValidLowLevel())
		return false;
	if (actor->IsPendingKill())
		return false;
	return true;
}
void Ulib::Destroy(AActor* actor)
{
	if (!Valid(actor))
		return;
	actor->Destroy();
	if (GEngine)
		GEngine->ForceGarbageCollection();
}
void Ulib::SetActorNI(AActor* actor)
{
	if (!Ulib::Valid(actor))
		return;
	actor->Tags.Add(ni_character_tag);
}
void Ulib::SetActorI(AActor* actor)
{
	if (!Ulib::Valid(actor))
		return;
	actor->Tags.Add(i_character_tag);
}

ACharacter* Ulib::GetEntityInVision(AActor* actor, float cone_radius, float deg)
{
	if (!Ulib::Valid(actor))
		return nullptr;
	UWorld* world = GEngine->GetWorldFromContextObject(actor, EGetWorldErrorMode::LogAndReturnNull);
	if (!world)
		return nullptr;

	TArray<AActor*> out_actors;
	FVector spherePos = actor->GetActorLocation() + actor->GetActorForwardVector() * cone_radius / 2.f;
	TArray<TEnumAsByte<EObjectTypeQuery>> object_types;
	object_types.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(actor);
	UKismetSystemLibrary::SphereOverlapActors(world, spherePos, cone_radius / 2.f, object_types, ACharacter::StaticClass(), IgnoredActors, out_actors);
	ACharacter* ret_entity = nullptr;
	FVector actor_loc = actor->GetActorLocation();
	for (AActor* _actor : out_actors)
	{
		if (!Ulib::Valid(_actor))
			continue;
		if (!IsHostile(actor, _actor))
			continue;
		FVector f_a = _actor->GetActorLocation() - actor_loc;
		f_a.Normalize();
		FVector f_b = actor->GetActorForwardVector();
		f_b.Normalize();
		f_a.Z = f_b.Z = 0.f;
		float f_dp = FVector::DotProduct(f_a, f_b);
		float adeg = FMath::RadiansToDegrees(f_dp);
		//UE_LOG(LogTemp, Warning, TEXT("%s %f %f"), *_actor->GetName(), f_dp, adeg);
		if (ret_entity == nullptr || FVector::Distance(_actor->GetActorLocation(), actor_loc) < FVector::Distance(ret_entity->GetActorLocation(), actor_loc)) {
			if(adeg < deg)
				ret_entity = Cast<ACharacter>(_actor);
		}
	}
	return ret_entity;
}

TArray<ACharacter*> Ulib::GetAllNI(UObject* contextObj)
{
	if (!contextObj)
		return {};
	UWorld* world = GEngine->GetWorldFromContextObject(contextObj, EGetWorldErrorMode::LogAndReturnNull);
	if (!world)
		return {};
	TArray<AActor*> out_actors;
	TArray<ACharacter*> ret_characters;
	UGameplayStatics::GetAllActorsOfClassWithTag(world, ACharacter::StaticClass(), FName(ni_character_tag), out_actors);
	UE_LOG(LogTemp, Warning, TEXT("Ni characters array size :%d"), out_actors.Num());
	for (AActor* actor : out_actors)
	{
		ACharacter* character = Cast<ACharacter>(actor);
		if (Ulib::Valid(character))
			ret_characters.Add(character);
	}
	return ret_characters;
}