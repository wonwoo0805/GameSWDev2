// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterBase.generated.h"

USTRUCT(BlueprintType)
struct FDestructiblePart
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Part")
    FName BoneName; // name of hitted bone "head", "left_arm"

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Part")
    float MaxHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Part")
    float CurrentHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Part")
    bool bIsDestroyed;

    // defalut constructer
    FDestructiblePart()
    {
        MaxHealth = 100.f;
        CurrentHealth = 100.f;
        bIsDestroyed = false;
    }
};



UCLASS()
class SOULLIKE_API AMonsterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Status")
	float MaxHealth;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Status")
	float CurrentHealth;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Destruction")
	TMap<FName,FDestructiblePart> DestructibleParts;

	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,class AController* EventInstigator,AActor* DamageCauser) override;
	
	AMonsterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
