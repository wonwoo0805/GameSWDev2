// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemObject.h"

UItemObject::UItemObject() {

}



void UItemObject::Add_ConsumeItem(int changeNum) {
	itemNum += changeNum;

	return;
}

int32 UItemObject::getItemNum() { return itemNum; }
int32 UItemObject::getItemMaxNum() { return itemMaxNum; }
UTexture2D* UItemObject::getItemIcon_2D() { return itemIcon_2D; };
UStaticMesh* UItemObject::getItemModel_3D() { return itemModel_3D; };
UAnimMontage* UItemObject::getItemAnimation_3D() { return itemAnimation_3D; };


