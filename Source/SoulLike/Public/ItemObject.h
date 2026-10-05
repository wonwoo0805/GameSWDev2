// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemObject.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SOULLIKE_API UItemObject : public UObject
{
	GENERATED_BODY()

public:
	
	UItemObject();
	int TryUse();
	void Add_ConsumeItem(int changeNum);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	int32 itemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	int32 itemMaxNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	int32 itemNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	FText itemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	UTexture2D* itemIcon_2D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	UStaticMesh* itemModel_3D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	UAnimMontage* itemAnimation_3D;

	virtual void UseItem();
	
};
