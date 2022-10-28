// Fill out your copyright notice in the Description page of Project Settings.


#include "evehud.h"
#include "lib.h"
#include "const.h"
#include "gameframework/playercontroller.h"
#include "kismet/gameplaystatics.h"

Aevehud::Aevehud()
{

}
void Aevehud::DrawHUD()
{
	Super::DrawHUD();
	/*APlayerController* controller = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!controller)
		return;
	int32 view_w, view_h;
	controller->GetViewportSize(view_w, view_h);
	DrawText(QuestText, FColor::Red, view_w / 6.f, view_h / 6.f);
	*/
}