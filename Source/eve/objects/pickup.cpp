// Fill out your copyright notice in the Description page of Project Settings.


#include "objects/pickup.h"
#include "components/scenecomponent.h"
#include "Components/TextRenderComponent.h"
#include "const.h"
#include "lib.h"
#include "kismet/gameplaystatics.h"
#include "character/evePlayer.h"

// Sets default values
Apickup::Apickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	rootScene = CreateDefaultSubobject<USceneComponent>("Root Scene Component");
	SetRootComponent(rootScene);

	//displayText = CreateDefaultSubobject<UTextRenderComponent>("Text Render Comp");
	//displayText->SetupAttachment(GetRootComponent());
	//
	//displayText->SetText(GetResourceName());
}

// Called when the game starts or when spawned
void Apickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void Apickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AActor* t_actor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Ulib::Valid(t_actor)) {
		float t_dst_actor = FVector::Distance(t_actor->GetActorLocation(), GetActorLocation());
		if (t_dst_actor <= pickupRange) {
			Use();
			//DisplayText();
			//FRotator t_look_at_player = (t_actor->GetActorLocation() - GetActorLocation()).Rotation();
			//displayText->SetWorldRotation(t_look_at_player);
		}
	}
}

void Apickup::DisplayText()
{
	if (m_bTextActive)
		return;
	m_bTextActive = 1;
	//displayText->SetHiddenInGame(false);
}
void Apickup::HideText()
{
	//displayText->SetHiddenInGame(true);
}
void Apickup::Use()
{
	AevePlayer* t_player = Cast<AevePlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Ulib::Valid(t_player)) {
		switch (resource) {
			case EResourceEnum::RESOURCE_NONE:
			{
				break;
			}
			case EResourceEnum::RESOURCE_AMMO:
			{
				int32 t_q = (int32)quantity;
				t_player->res.ammo += t_q;
				break;
			}
			case EResourceEnum::RESOURCE_MEDIPACK:
			{
				float t_new_hp = t_player->health + quantity;
				if (t_new_hp > 100.f)
					t_new_hp = 100.f;
				t_player->health = t_new_hp;
				break;
				
			}
			default:
				break;

		}
	}
	Ulib::Destroy(this);
}
FText Apickup::GetResourceName()
{
	switch (resource) {
		case EResourceEnum::RESOURCE_NONE:
		{
			return FText::FromString(" ");
			break;
		}
		case EResourceEnum::RESOURCE_AMMO:
		{
			return FText::FromString("Ammo");
			break;
		}
		case EResourceEnum::RESOURCE_MEDIPACK:
		{
			return FText::FromString("Medipack");
			break;

		}
		default:
			break;

	}
	return FText::FromString("Something");
}