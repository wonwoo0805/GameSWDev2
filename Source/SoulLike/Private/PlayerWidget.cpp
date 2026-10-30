// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"
#include "Components/ProgressBar.h"
#include "UsableItem.h"

UPlayerWidget::UPlayerWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UPlayerWidget::UpdatePlayerHPSP(int newHp, int newSp) {
	if (HP_Bar) {
		float hpPercentage = (float)newHp / (float)MaxHp;

		HP_Bar->SetPercent(hpPercentage);
	}

	if (SP_Bar) {
		float spPercentage = (float)newSp / (float)MaxSp;

		SP_Bar->SetPercent(spPercentage);
	}
}

void UPlayerWidget::UpdatePlayerStats(int newAtk, int newSpd, int newExtraStat) {

}

void UPlayerWidget::UpdateItemSlots(const TArray<UUsableItem*>& PlayerInventory, int32 CenterSlotIndex) {
	int currentItemNum = -1;
	
	if (CenterSlotIndex == 0) { // if middleItem is HP_Potion
		MiddleItemName->SetText(FText::FromString(TEXT("Heals")));
		currentItemNum = PlayerInventory[CenterSlotIndex]->GetItemNum();
		if (currentItemNum == 0) {
			HPPotionSlot->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->GetItemIcon_Empty_2D()); //emptyImg
		}
		else if (currentItemNum > 0 && currentItemNum < PlayerInventory[CenterSlotIndex]->GetItemMaxNum()) {
			HPPotionSlot->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->GetItemIcon_Half_2D()); //halfImg
		}
		else {
			currentItemNum = PlayerInventory[CenterSlotIndex]->GetItemMaxNum();
			HPPotionSlot->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->GetItemIcon_2D()); //fullImg
		}

		//좌 우 아이템슬롯의 투명도(?)를 조절하는 머테리얼(?)을 설정
		MPPotionSlot->SetBrushFromTexture(PlayerInventory[1]->GetItemIcon_2D());
		GenerationPotionSlot->SetBrushFromTexture(PlayerInventory[2]->GetItemIcon_2D());
	}
	else if (CenterSlotIndex == 1) { // if middleItem is MP_Potion
		MiddleItemName->SetText(FText::FromString(TEXT("Manas")));
		currentItemNum = PlayerInventory[CenterSlotIndex]->GetItemNum();
		if (currentItemNum == 0) {
			MPPotionSlot->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->GetItemIcon_Empty_2D()); //emptyImg
		}
		else if (currentItemNum > 0 && currentItemNum < PlayerInventory[CenterSlotIndex]->GetItemMaxNum()) {
			MPPotionSlot->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->GetItemIcon_Half_2D()); //halfImg
		}
		else {
			currentItemNum = PlayerInventory[CenterSlotIndex]->GetItemMaxNum();
			MPPotionSlot->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->GetItemIcon_2D()); //fullImg
		}

		HPPotionSlot->SetBrushFromTexture(PlayerInventory[0]->GetItemIcon_2D());
		GenerationPotionSlot->SetBrushFromTexture(PlayerInventory[2]->GetItemIcon_2D());
	}
	else if (CenterSlotIndex == 2) { // if middleItem is Generation_Potion
		MiddleItemName->SetText(FText::FromString(TEXT("Generations")));
		currentItemNum = PlayerInventory[CenterSlotIndex]->GetItemNum();
		if (currentItemNum == 0) {
			GenerationPotionSlot->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->GetItemIcon_Empty_2D()); //emptyImg
		}
		else if (currentItemNum > 0 && currentItemNum < PlayerInventory[CenterSlotIndex]->GetItemMaxNum()) {
			GenerationPotionSlot->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->GetItemIcon_Half_2D()); //halfImg
		}
		else {
			currentItemNum = PlayerInventory[CenterSlotIndex]->GetItemMaxNum();
			GenerationPotionSlot->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->GetItemIcon_2D()); //fullImg
		}

		HPPotionSlot->SetBrushFromTexture(PlayerInventory[0]->GetItemIcon_2D());
		MPPotionSlot->SetBrushFromTexture(PlayerInventory[1]->GetItemIcon_2D());
	}
	
	if (MiddleItemNum != nullptr && currentItemNum >= 0)
	{
		MiddleItemNum->SetText(FText::AsNumber(currentItemNum));
	}

}