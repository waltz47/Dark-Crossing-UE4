//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "worldScript.h"
//#include "lib.h"
//#include "character/eveped.h"
//#include "kismet/gameplaystatics.h"
//#include "evehud.h"
//#include "components/spherecomponent.h"
//#include "character/aidirector.h"
//// Sets default values
//AworldScript::AworldScript()
//{
// 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = false;
//	collisionSphere = CreateDefaultSubobject<USphereComponent>("Sphere Collision Comp");
//	SetRootComponent(collisionSphere);
//
//	collisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AworldScript::OnOverlapBegin);
//}
//
//// Called when the game starts or when spawned
//void AworldScript::BeginPlay()
//{
//	Super::BeginPlay();
//}
//
//void AworldScript::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
//{
//	AActor* t_player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
//	if (Ulib::Valid(t_player) && Ulib::Valid(OtherActor) && t_player == OtherActor) {
//		ActivateFunc();
//	}
//}
//void AworldScript::ActivateFunc()
//{
//	for (Aaidirector* t_director : activateActors) {
//		if (Ulib::Valid(t_director)) {
//			t_director->Activate();
//		}
//	}
//	Ulib::Destroy(this);
//}
