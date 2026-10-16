// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemObject.h"
#include "UsableItem.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UUsableItem : public UItemObject
{
	GENERATED_BODY()
	
public:
	int TryUse();

	UFUNCTION(BlueprintPure, Category = "Item Data")
	int32 GetItemNum() const { return itemNum; }

	UFUNCTION(BlueprintPure, Category = "Item Data")
	UTexture2D* GetItemIcon() const { return itemIcon_2D; }

	UFUNCTION(BlueprintPure, Category = "Item Data")
	FText GetItemName() const { return itemName; }

protected:


	virtual void UseItem();

};
