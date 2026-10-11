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

protected:


	virtual void UseItem();

};
