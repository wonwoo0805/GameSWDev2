// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemObject.h"
#include "Heals.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UHeals : public UItemObject
{
	GENERATED_BODY()
	
public:

	virtual void UseItem() override;

protected:


private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Heals Data")
	int healAmount;

};
