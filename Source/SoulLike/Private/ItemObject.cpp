// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemObject.h"

UItemObject::UItemObject() {

}



void UItemObject::Add_ConsumeItem(int changeNum) {
	itemNum += changeNum;

	return;
}

int32 UItemObject::getItemNum() { return itemNum; }


