// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulLikeCharacter.h"
#include "ItemObject.h"
#include "UsableItem.h"

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

