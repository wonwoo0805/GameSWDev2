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
	/*
	if (CenterSlotIndex == 0) { // if middleItem is HP_Potion
		MiddleItemName->SetText("Heals");
		currentItemNum = PlayerInventory[CenterSlotIndex]->GetItemNum();
		if (currentItemNum == 0) {
			HPPotionImg->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->getItemIcon_2D()); //emptyImg
		}
		else if (currentItemNum > 0 && currentItemNum < PlayerInventory[CenterSlotIndex]->getItemMaxNum()) {
			HPPotionImg->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->getItemIcon_2D()); //halfImg
		}
		else {
			currentItemNum = PlayerInventory[CenterSlotIndex]->getItemMaxNum();
			HPPotionImg->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->getItemIcon_2D()); //fullImg
		}
		//좌 우 아이템슬롯의 투명도(?)를 조절하는 머테리얼(?)을 설정
		//PlayerInventory[1]->SetBrushFromMaterial() 
		//PlayerInventory[2]->SetBrushFromMaterial() 
	}
	else if (CenterSlotIndex == 1) { // if middleItem is MP_Potion
		MiddleItemName->SetText("Manas");
		currentItemNum = PlayerInventory[CenterSlotIndex]->GetItemNum();
		if (currentItemNum == 0) {
			MPPotionImg->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->getItemIcon_2D()); //emptyImg
		}
		else if (currentItemNum > 0 && currentItemNum < PlayerInventory[CenterSlotIndex]->getItemMaxNum()) {
			MPPotionImg->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->getItemIcon_2D()); //halfImg
		}
		else {
			PlayerInventory[CenterSlotIndex]->getItemMaxNum();
			MPPotionImg->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->getItemIcon_2D()); //fullImg
		}
	}
	else if (CenterSlotIndex == 2) { // if middleItem is Generation_Potion
		MiddleItemName->SetText("Generations");
		currentItemNum = PlayerInventory[CenterSlotIndex]->GetItemNum();
		if (currentItemNum == 0) {
			GenerationPotionImg->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->getItemIcon_2D()); //emptyImg
		}
		else if (currentItemNum > 0 && currentItemNum < PlayerInventory[CenterSlotIndex]->getItemMaxNum()) {
			GenerationPotionImg->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->getItemIcon_2D()); //halfImg
		}
		else {
			PlayerInventory[CenterSlotIndex]->getItemMaxNum();
			GenerationPotionImg->SetBrushFromTexture(PlayerInventory[CenterSlotIndex]->getItemIcon_2D()); //fullImg
		}
	}
	*/
}