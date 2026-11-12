// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"
#include "Components/ProgressBar.h"
#include "UsableItem.h"

/*------------------------------------------------------------------*/
// [Added Code] Headers for math interpolation and dynamic materials
#include "Math/UnrealMathUtility.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
/*------------------------------------------------------------------*/

UPlayerWidget::UPlayerWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

/*------------------------------------------------------------------*/
// [Added Code] Perspective carousel movement and dynamic material application for side slots
void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UImage* SlotImages[3] = { HPPotionSlot, MPPotionSlot, GenerationPotionSlot };

	// Initial layout in Designer: [0: HPPotionSlot = Center, 1: MPPotionSlot = Left, 2: GenerationPotionSlot = Right]
	for (int32 i = 0; i < 3; ++i)
	{
		const float InitialOffsetY = (i == 0) ? 0.0f : SideSlotOffsetY;
		CurrentTranslations[i] = FVector2D(0.0f, InitialOffsetY);
		TargetTranslations[i] = CurrentTranslations[i];

		CurrentScales[i] = (i == 0) ? CenterSlotScale : SideSlotScale;
		TargetScales[i] = CurrentScales[i];

		CurrentOpacities[i] = (i == 0) ? 1.0f : SideSlotOpacity;
		TargetOpacities[i] = CurrentOpacities[i];

		if (SlotImages[i])
		{
			SlotImages[i]->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
			SlotImages[i]->SetRenderTranslation(CurrentTranslations[i]);
			SlotImages[i]->SetRenderScale(FVector2D(CurrentScales[i], CurrentScales[i]));
			SlotImages[i]->SetRenderOpacity(CurrentOpacities[i]);
		}
	}
}

void UPlayerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsMovingSlots) return;

	UImage* SlotImages[3] = { HPPotionSlot, MPPotionSlot, GenerationPotionSlot };
	bool bAllArrived = true;

	for (int32 i = 0; i < 3; ++i)
	{
		if (!SlotImages[i]) continue;

		CurrentTranslations[i] = FMath::Vector2DInterpTo(CurrentTranslations[i], TargetTranslations[i], InDeltaTime, SlotInterpSpeed);
		CurrentScales[i] = FMath::FInterpTo(CurrentScales[i], TargetScales[i], InDeltaTime, SlotInterpSpeed);
		CurrentOpacities[i] = FMath::FInterpTo(CurrentOpacities[i], TargetOpacities[i], InDeltaTime, SlotInterpSpeed);

		SlotImages[i]->SetRenderTranslation(CurrentTranslations[i]);
		SlotImages[i]->SetRenderScale(FVector2D(CurrentScales[i], CurrentScales[i]));
		SlotImages[i]->SetRenderOpacity(CurrentOpacities[i]);

		if (FVector2D::Distance(CurrentTranslations[i], TargetTranslations[i]) > 0.5f)
		{
			bAllArrived = false;
		}
	}

	if (bAllArrived)
	{
		for (int32 i = 0; i < 3; ++i)
		{
			if (!SlotImages[i]) continue;

			CurrentTranslations[i] = TargetTranslations[i];
			CurrentScales[i] = TargetScales[i];
			CurrentOpacities[i] = TargetOpacities[i];

			SlotImages[i]->SetRenderTranslation(CurrentTranslations[i]);
			SlotImages[i]->SetRenderScale(FVector2D(CurrentScales[i], CurrentScales[i]));
			SlotImages[i]->SetRenderOpacity(CurrentOpacities[i]);
		}
		bIsMovingSlots = false;
	}
}

void UPlayerWidget::MoveItemSlots(const TArray<UUsableItem*>& PlayerInventory, int32 NewCenterSlotIndex)
{
	// Home lanes in Designer: Index 0 (HP) = 0 (Center), Index 1 (MP) = -1 (Left), Index 2 (Generation) = +1 (Right)
	const int32 HomeLanes[3] = { 0, -1, 1 };

	const int32 CenterIdx = (NewCenterSlotIndex % 3 + 3) % 3;
	const int32 RightIdx = (CenterIdx + 2) % 3;
	const int32 LeftIdx = (CenterIdx + 1) % 3;

	int32 TargetLanes[3] = { 0, 0, 0 };
	TargetLanes[CenterIdx] = 0;  // Move to Center lane
	TargetLanes[LeftIdx] = -1;   // Move to Left lane
	TargetLanes[RightIdx] = 1;   // Move to Right lane

	for (int32 i = 0; i < 3; ++i)
	{
		const float OffsetX = (float)(TargetLanes[i] - HomeLanes[i]) * SlotSpacing;
		const float OffsetY = (i == CenterIdx) ? 0.0f : SideSlotOffsetY;

		TargetTranslations[i] = FVector2D(OffsetX, OffsetY);
		TargetScales[i] = (i == CenterIdx) ? CenterSlotScale : SideSlotScale;
		TargetOpacities[i] = (i == CenterIdx) ? 1.0f : SideSlotOpacity;
	}

	bIsMovingSlots = true;
	UpdateItemSlots(PlayerInventory, CenterIdx);
}

void UPlayerWidget::UpdateSingleSlotVisual(UImage* TargetSlot, UUsableItem* Item, bool bIsCenterSlot)
{
	if (!TargetSlot || !Item) return;

	const int32 ItemNum = Item->GetItemNum();
	const int32 ItemMaxNum = Item->GetItemMaxNum();

	UTexture2D* SelectedTexture = nullptr;
	if (ItemNum <= 0)
	{
		SelectedTexture = Item->GetItemIcon_Empty_2D(); // emptyImg
	}
	else if (ItemNum < ItemMaxNum)
	{
		SelectedTexture = Item->GetItemIcon_Half_2D();  // halfImg
	}
	else
	{
		SelectedTexture = Item->GetItemIcon_2D();       // fullImg
	}

	// Center slot uses clean texture; Side slots use the ItemSlot gradient material if assigned
	if (bIsCenterSlot || SideSlotMaterial == nullptr)
	{
		TargetSlot->SetBrushFromTexture(SelectedTexture);
	}
	else
	{
		TargetSlot->SetBrushFromMaterial(SideSlotMaterial);
		if (UMaterialInstanceDynamic* DynMat = TargetSlot->GetDynamicMaterial())
		{
			DynMat->SetTextureParameterValue(MaterialTextureParamName, SelectedTexture);
		}
	}
}
/*------------------------------------------------------------------*/

void UPlayerWidget::UpdatePlayerHPSP(int newHp, int newSp) {
	if (HP_Bar) {
		float hpPercentage = (float)newHp / (float)MaxHp;

		HP_Bar->SetPercent(hpPercentage);
	}

	if (SP_Bar) {
		float spPercentage = (float)newSp / (float)MaxSp;

		SP_Bar->SetPercent(spPercentage);
	}
}

void UPlayerWidget::UpdatePlayerStats(int newAtk, int newSpd, int newExtraStat) {

}

void UPlayerWidget::UpdateItemSlots(const TArray<UUsableItem*>& PlayerInventory, int32 CenterSlotIndex) {
	int currentItemNum = -1;

	/*------------------------------------------------------------------*/
	// [Updated Code] Apply clean texture to Center slot, and gradient material + texture to Side slots
	if (PlayerInventory.IsValidIndex(0))
	{
		UpdateSingleSlotVisual(HPPotionSlot, PlayerInventory[0], (CenterSlotIndex == 0));
	}
	if (PlayerInventory.IsValidIndex(1))
	{
		UpdateSingleSlotVisual(MPPotionSlot, PlayerInventory[1], (CenterSlotIndex == 1));
	}
	if (PlayerInventory.IsValidIndex(2))
	{
		UpdateSingleSlotVisual(GenerationPotionSlot, PlayerInventory[2], (CenterSlotIndex == 2));
	}
	/*------------------------------------------------------------------*/

	if (CenterSlotIndex == 0) { // if middleItem is HP_Potion
		MiddleItemName->SetText(FText::FromString(TEXT("Heals")));
		currentItemNum = PlayerInventory[CenterSlotIndex]->GetItemNum();
		if (currentItemNum > PlayerInventory[CenterSlotIndex]->GetItemMaxNum()) {
			currentItemNum = PlayerInventory[CenterSlotIndex]->GetItemMaxNum();
		}
	}
	else if (CenterSlotIndex == 1) { // if middleItem is MP_Potion
		MiddleItemName->SetText(FText::FromString(TEXT("Manas")));
		currentItemNum = PlayerInventory[CenterSlotIndex]->GetItemNum();
		if (currentItemNum > PlayerInventory[CenterSlotIndex]->GetItemMaxNum()) {
			currentItemNum = PlayerInventory[CenterSlotIndex]->GetItemMaxNum();
		}
	}
	else if (CenterSlotIndex == 2) { // if middleItem is Generation_Potion
		MiddleItemName->SetText(FText::FromString(TEXT("Generations")));
		currentItemNum = PlayerInventory[CenterSlotIndex]->GetItemNum();
		if (currentItemNum > PlayerInventory[CenterSlotIndex]->GetItemMaxNum()) {
			currentItemNum = PlayerInventory[CenterSlotIndex]->GetItemMaxNum();
		}
	}

	if (MiddleItemNum != nullptr && currentItemNum >= 0)
	{
		MiddleItemNum->SetText(FText::AsNumber(currentItemNum));
	}

}