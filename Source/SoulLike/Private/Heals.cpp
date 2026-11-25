// Fill out your copyright notice in the Description page of Project Settings.


#include "Heals.h"
#include "../SoulLikeCharacter.h"

void UHeals::UseItem(ASoulLikeCharacter* thisCharacter) {
	//캐릭터의 체력 회복

	//thisCharacter->heals(healAmount_HP);

	Add_ConsumeItem(-1);

	if (thisCharacter != nullptr)
	{
		thisCharacter->HealHP(healAmount_HP);
	}

	Super::UseItem(thisCharacter);

	return;
}