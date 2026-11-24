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
	UFUNCTION(BlueprintCallable, Category = "Item")
	bool TryUse(ASoulLikeCharacter* thisCharacter);

	UFUNCTION(BlueprintPure, Category = "Item Data")
	int32 GetItemNum() const { return itemNum; }

	UFUNCTION(BlueprintPure, Category = "Item Data")
	UTexture2D* GetItemIcon() const { return itemIcon_2D; }

	UFUNCTION(BlueprintPure, Category = "Item Data")
	FText GetItemName() const { return itemName; }

	UFUNCTION(BlueprintPure, Category = "Item Data")
	UTexture2D* GetItemIcon_Half_2D() const { return itemIcon_Half_2D; }

	UFUNCTION(BlueprintPure, Category = "Item Data")
	UTexture2D* GetItemIcon_Empty_2D() const { return itemIcon_Empty_2D; }

protected:
	virtual void UseItem(ASoulLikeCharacter* thisCharacter) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	UTexture2D* itemIcon_Half_2D;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	UTexture2D* itemIcon_Empty_2D;

};
