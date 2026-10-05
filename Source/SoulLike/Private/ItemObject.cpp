// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemObject.h"

UItemObject::UItemObject() {

}

int UItemObject::TryUse() {
	if (itemNum <= 0) {
		return 0;
	}

	//if(don't move, jump, attack) return 0;

	return 1;
}

void UItemObject::Add_ConsumeItem(int changeNum) {
	itemNum += changeNum;

	return;
}

void UItemObject::UseItem() {
	//have no function
}


