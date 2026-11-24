// Fill out your copyright notice in the Description page of Project Settings.


#include "Heals_MP.h"

void UHeals_MP::UseItem(ASoulLikeCharacter* thisCharacter) {

	

	Add_ConsumeItem(-1);

	Super::UseItem(thisCharacter);

	return;
}


