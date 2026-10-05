// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
class UProgressBar;


UCLASS()
class SOULLIKE_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	UPlayerWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	int32 MaxHp = 700;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	int32 MaxSp = 500;

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdatePlayerHPSP(int newHp, int newSp);
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdatePlayerStats(int newAtk, int newSpd, int newExtraStat);
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateItemSlots();
	
protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HP_Bar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* SP_Bar;

	



	
};
