// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulLikeCharacter.h"
#include "ItemObject.h"
#include "UsableItem.h"
#include "PlayerWidget.h"
#include "ESCWidget.h"

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
			MaxHealth = PlayerWidget->MaxHp;
			Health = MaxHealth;

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

float ASoulLikeCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 1. 구르기 중(무적 프레임)이거나 이미 사망했으면 데미지 무시
	if (bIsDodging || Health <= 0.f)
	{
		return 0.f;
	}

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage <= 0.f)
	{
		return 0.f;
	}

	// 2. 체력 차감 및 0 미만 방지
	Health = FMath::Clamp(Health - ActualDamage, 0.f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Player Damaged! CurrentHealth: %f / %f"), Health, MaxHealth);

	// 3. 상단 HP/SP 위젯 갱신 (현재 SP 변수가 있으면 함께 전달)
	if (PlayerWidget != nullptr)
	{
		PlayerWidget->UpdatePlayerHPSP(Health, PlayerWidget->MaxSp);
	}

	// 4. 사망 처리
	if (Health <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Died!"));
		// 필요 시 사망 몽타주 재생이나 입력 비활성화 추가
	}

	return ActualDamage;
}

void ASoulLikeCharacter::UseCurrentItem()
{
	if (!Inventory.IsValidIndex(CenterSlotIndex) || Inventory[CenterSlotIndex] == nullptr)
	{
		return;
	}

	// Execute UseItem on the item currently in the center slot
	//Inventory[CenterSlotIndex]->UseItem(this);

	// Refresh slot icons (Full -> Half -> Empty) and remaining count text
	if (PlayerWidget != nullptr)
	{
		PlayerWidget->UpdateItemSlots(Inventory, CenterSlotIndex);
	}
}

void ASoulLikeCharacter::TogglePauseMenu()
{
	if (PauseMenuWidget == nullptr && PauseMenuClass != nullptr)
	{
		PauseMenuWidget = CreateWidget<UESCWidget>(GetWorld(), PauseMenuClass);
	}

	if (PauseMenuWidget != nullptr)
	{
		PauseMenuWidget->TogglePauseMenu();
	}
}

void ASoulLikeCharacter::HealHP(float Amount)
{
	if (bIsDead || Amount <= 0.0f) return;

	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);

	if (PlayerWidget != nullptr)
	{
		PlayerWidget->UpdatePlayerHPSP(Health, Stamina);
	}
}
/*
void ASoulLikeCharacter::RestoreMP(float Amount)
{
	if (bIsDead || Amount <= 0.0f) return;

	Mana = FMath::Clamp(Mana + Amount, 0.0f, MaxMana);

	if (PlayerWidget != nullptr)
	{
		PlayerWidget->UpdatePlayerMP(Mana);
	}
}

void ASoulLikeCharacter::StartRegenHP(float AmountPerSecond, int32 DurationSeconds)
{
	if (bIsDead || DurationSeconds <= 0) return;

	// Clear any existing regen timer so drinking again cleanly refreshes the 8-second duration
	GetWorldTimerManager().ClearTimer(RegenTimerHandle);

	RegenAmountPerTick = AmountPerSecond;
	RemainingRegenTicks = DurationSeconds;

	// Call OnRegenTick every 1.0 second (looping)
	GetWorldTimerManager().SetTimer(
		RegenTimerHandle,
		this,
		&ASoulLikeCharacter::OnRegenTick,
		1.0f,
		true
	);
}
*/