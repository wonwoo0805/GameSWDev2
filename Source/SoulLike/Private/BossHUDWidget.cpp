// Fill out your copyright notice in the Description page of Project Settings.

#include "BossHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Math/UnrealMathUtility.h"

void UBossHUDWidget::InitBossInfo(const FText& InBossName, float CurrentHP, float MaxHP)
{
	if (BossNameText)
	{
		BossNameText->SetText(InBossName);
	}

	const float Ratio = (MaxHP > 0.0f) ? FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f) : 0.0f;
	CurrentHPPercent = Ratio;
	TargetHPPercent = Ratio;

	if (BossHP_Bar)
	{
		BossHP_Bar->SetPercent(CurrentHPPercent);
	}
}

void UBossHUDWidget::UpdateBossHP(float CurrentHP, float MaxHP)
{
	TargetHPPercent = (MaxHP > 0.0f) ? FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f) : 0.0f;
}

void UBossHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (BossHP_Bar && !FMath::IsNearlyEqual(CurrentHPPercent, TargetHPPercent, 0.001f))
	{
		CurrentHPPercent = FMath::FInterpTo(CurrentHPPercent, TargetHPPercent, InDeltaTime, BarInterpSpeed);
		BossHP_Bar->SetPercent(CurrentHPPercent);
	}
}