// Fill out your copyright notice in the Description page of Project Settings.


#include "Heals_MP.h"
#include "../SoulLikeCharacter.h"

void UHeals_MP::UseItem(ASoulLikeCharacter* thisCharacter) {

	

	Add_ConsumeItem(-1);

	if (thisCharacter != nullptr)
	{
		thisCharacter->RestoreMP(healAmount_MP);
	}

	Super::UseItem(thisCharacter);

	return;
}


