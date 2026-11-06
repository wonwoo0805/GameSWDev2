// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulLikeCharacter.h"
#include "ItemObject.h"
#include "UsableItem.h"
#include "PlayerWidget.h"

// Sets default values
ASoulLikeCharacter::ASoulLikeCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASoulLikeCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	
	Inventory.SetNum(3);
    if (Slot0_ItemClass)
    {
        Inventory[0] = NewObject<UUsableItem>(this, Slot0_ItemClass);
    }
    if (Slot1_ItemClass)
    {
        Inventory[1] = NewObject<UUsableItem>(this, Slot1_ItemClass);
    }
    if (Slot2_ItemClass)
    {
        Inventory[2] = NewObject<UUsableItem>(this, Slot2_ItemClass);
    }

	if (PlayerWidgetClass != nullptr)
	{
		// 틀(PlayerWidgetClass)을 바탕으로 실제 위젯을 생성해 리모컨(PlayerWidget)에 저장
		PlayerWidget =   CreateWidget<UPlayerWidget>(GetWorld(), PlayerWidgetClass);

		if (PlayerWidget != nullptr)
		{
			// 화면에 위젯 부착 (블루프린트의 Add to Viewport 노드와 완전히 동일한 역할)
			PlayerWidget->AddToViewport();

			// [순서 3] 위젯이 화면에 떴고 인벤토리도 준비되었으니 첫 화면 상태 초기화!
			CenterSlotIndex = 0; // 시작 시 0번(HP 포션)을 중앙 슬롯으로 설정

			// 체력/스태미나 바를 가득 찬 상태로 초기화 (위젯 내부의 MaxHp, MaxSp 기준)
			PlayerWidget->UpdatePlayerHPSP(PlayerWidget->MaxHp, PlayerWidget->MaxSp);

			// 준비된 인벤토리 배열과 중앙 슬롯 번호(0)를 넘겨 슬롯 아이콘/이름/개수 초기화
			PlayerWidget->UpdateItemSlots(Inventory, CenterSlotIndex);
		}
	}
    //UpdateItemSlots();



}

// Called every frame
void ASoulLikeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASoulLikeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASoulLikeCharacter::ChangeCenterSlot(int32 Direction)
{
	// 1. 0, 1, 2 사이를 순환하도록 중앙 슬롯 인덱스 변경
	// (+3을 더해준 뒤 % 3을 하면 음수가 나와도 0 -> 2 -> 1 -> 0으로 안전하게 순환합니다)
	CenterSlotIndex = (CenterSlotIndex + Direction + 3) % 3;

	// 2. 위젯의 슬롯 이동 함수 실행 (이동 함수 내부 마지막에 UpdateItemSlots가 호출됨)
	if (PlayerWidget != nullptr)
	{
		PlayerWidget->MoveItemSlots(Inventory, CenterSlotIndex);
	}
}