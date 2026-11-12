// Fill out your copyright notice in the Description page of Project Settings.


#include "Heals.h"

void UHeals::UseItem(ASoulLikeCharacter* thisCharacter) {
	//사용할 수 있는가에 대해서는 ItemObject의 TryUse에서 판정
	//마시는 모션 재생
	//캐릭터의 체력 회복
	if (!TryUse()) { return; }

	//thisCharacter->heals(healAmount_HP);

	Add_ConsumeItem(-1);

	return;
}