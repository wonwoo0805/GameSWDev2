// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class SOULLIKE_API UBossHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Smooth interpolation speed for the boss HP bar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Boss")
	float BarInterpSpeed = 6.0f;

	UFUNCTION(BlueprintCallable, Category = "UI|Boss")
	void InitBossInfo(const FText& InBossName, float CurrentHP, float MaxHP);

	UFUNCTION(BlueprintCallable, Category = "UI|Boss")
	void UpdateBossHP(float CurrentHP, float MaxHP);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* BossHP_Bar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BossNameText;

private:
	float CurrentHPPercent = 1.0f;
	float TargetHPPercent = 1.0f;
};