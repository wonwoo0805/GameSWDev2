// Fill out your copyright notice in the Description page of Project Settings.


#include "UsableItem.h"
#include "../SoulLikeCharacter.h"

int UUsableItem::TryUse() {
	if (itemNum <= 0) {
		return 0;
	}

	//if(don't move, jump, attack) return 0;

	return 1;
}

void UUsableItem::UseItem() {
	//have no function
}



