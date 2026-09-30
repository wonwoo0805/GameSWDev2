// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"

#include "Components/ProgressBar.h"

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

void UPlayerWidget::UpdateItemSlots() {

}