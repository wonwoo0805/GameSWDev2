// Fill out your copyright notice in the Description page of Project Settings.


#include "UsableItem.h"
#include "../SoulLikeCharacter.h"

bool UUsableItem::TryUse(ASoulLikeCharacter* thisCharacter) {
	if (itemNum <= 0) {
		return false;
	}

	//if(jump, dodge, attack, potion, hit, dead) return 0;

	thisCharacter->bIsUsingItem = true;

	return true;
}

void UUsableItem::UseItem(ASoulLikeCharacter* thisCharacter) {
	thisCharacter->bIsUsingItem = false;

	return;
}



