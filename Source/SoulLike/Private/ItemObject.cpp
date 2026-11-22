// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemObject.h"

UItemObject::UItemObject() {

}



void UItemObject::Add_ConsumeItem(int changeNum) {
	itemNum += changeNum;

	return;
}

int32 UItemObject::GetItemNum() { return itemNum; }
int32 UItemObject::GetItemMaxNum() { return itemMaxNum; }
UTexture2D* UItemObject::GetItemIcon_2D() { return itemIcon_2D; };
UStaticMesh* UItemObject::GetItemModel_3D() { return itemModel_3D; };
UAnimMontage* UItemObject::GetItemAnimation_3D() { return itemAnimation_3D; };


