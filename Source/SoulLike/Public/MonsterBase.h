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
    UFUNCTION(BlueprintImplementableEvent, Category = "Destructible")
	void OnPartDestroyed(FName BrokenBoneName);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    float LastAttackTime = -100.f;

	// 몽타주 종료 시 호출될 콜백
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    // 몬스터 기본 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage = 20.f;

	// 공격 사거리 (이 거리 안으로 플레이어가 오면 공격 시도)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange = 250.f;

	// 공격 후 다음 공격까지의 대기 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackCooldown = 2.0f;

	// 사용할 공격 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	// 현재 공격 모션 재생 중인지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking = false;

	// 공격 실행 함수 (블루프린트나 AI에서도 호출 가능)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformAttack();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackAngleThreshold = 35.f;

	// 플레이어를 발견하고 쳐다보기 시작하는 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DetectRange = 1200.f;

	// 본격적으로 추적을 시작하는 거리 (DetectRange보다 작고 AttackRange보다 커야 함)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float ChaseRange = 700.f;

	// 추적 시 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float ChaseSpeed = 300.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsDead = false;

	// 사망 시 드롭할 소재/아이템 액터 클래스 (블루프린트에서 지정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
	TSubclassOf<AActor> DropItemClass;

	// 사망 처리 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void Die();

	// 블루프린트에서 추가 연출(사운드, 파티클, 다중 드롭 테이블 등)을 붙일 때 사용
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnMonsterDied();
};
