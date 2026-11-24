// Fill out your copyright notice in the Description page of Project Settings.


#include "Heals_Generation.h"


void UHeals_Generation::UseItem(ASoulLikeCharacter* thisCharacter) {
	Add_ConsumeItem(-1);

	Super::UseItem(thisCharacter);

	return;
}


