// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PlayerWidget.generated.h"

/**
 *
 */
class UUsableItem;
class UProgressBar;
class UMaterialInterface;


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
	void UpdateItemSlots(const TArray<UUsableItem*>& PlayerInventory, int32 CenterSlotIndex);

	/*------------------------------------------------------------------*/
	// [Added Code] Carousel movement, perspective offset/scale, and side slot material settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|SlotCarousel")
	float SlotInterpSpeed = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|SlotCarousel")
	float SlotSpacing = 112.0f;

	// Negative Y moves the side slots higher up on screen for 3D perspective depth
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|SlotCarousel")
	float SideSlotOffsetY = -25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|SlotCarousel")
	float CenterSlotScale = 1.15f;

	// Smaller scale for left/right slots to enhance perspective depth
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|SlotCarousel")
	float SideSlotScale = 0.72f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|SlotCarousel")
	float SideSlotOpacity = 0.65f;

	// Assign the "ItemSlot" UI material here in WBP_PlayerHUD Details panel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|SlotCarousel")
	UMaterialInterface* SideSlotMaterial = nullptr;

	// Name of the TextureSampleParameter2D node inside the ItemSlot material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|SlotCarousel")
	FName MaterialTextureParamName = FName("ItemTexture");

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void MoveItemSlots(const TArray<UUsableItem*>& PlayerInventory, int32 NewCenterSlotIndex);
	/*------------------------------------------------------------------*/

protected:
	/*------------------------------------------------------------------*/
	// [Added Code] RenderTransform-based interpolation variables and per-slot visual helper
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdateSingleSlotVisual(UImage* TargetSlot, UUsableItem* Item, bool bIsCenterSlot);

	FVector2D CurrentTranslations[3];
	FVector2D TargetTranslations[3];

	float CurrentScales[3];
	float TargetScales[3];

	float CurrentOpacities[3];
	float TargetOpacities[3];

	bool bIsMovingSlots = false;
	/*------------------------------------------------------------------*/

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HP_Bar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* SP_Bar;

	UPROPERTY(meta = (BindWidget))
	UImage* HPPotionSlot;

	UPROPERTY(meta = (BindWidget))
	UImage* MPPotionSlot;

	UPROPERTY(meta = (BindWidget))
	UImage* GenerationPotionSlot;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MiddleItemName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MiddleItemNum;
};