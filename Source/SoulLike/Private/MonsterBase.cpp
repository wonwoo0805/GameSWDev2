// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterBase.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMonsterBase::AMonsterBase()
{
	MaxHealth = 1000.f;
	CurrentHealth = MaxHealth;
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentHealth <= 0.f)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		return;
	}

	// 공격 중이 아닐 때만 플레이어 방향으로 회전
	if (!bIsAttacking)
	{
		FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerPawn->GetActorLocation());
		TargetRot.Pitch = 0.f;
		TargetRot.Roll = 0.f;

		FRotator SmoothRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 3.0f);
		SetActorRotation(SmoothRot);

		// 사거리 이내이고 쿨타임이 지났으면 공격 실행
		const float DistToPlayer = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
		const float CurrentTime = GetWorld()->GetTimeSeconds();
		if (DistToPlayer <= AttackRange && (CurrentTime - LastAttackTime) >= AttackCooldown)
		{
			PerformAttack();
		}
	}
}

// Called to bind functionality to input
void AMonsterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AMonsterBase::TakeDamage(float DamageAmount,FDamageEvent const& DamageEvent,AController* EventInstigator,AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount,DamageEvent,EventInstigator,DamageCauser);
	UE_LOG(LogTemp,Warning,TEXT("HIT"));
	if(ActualDamage <= 0.f || CurrentHealth <= 0.f)
	{
		return 0.f;
	}

	CurrentHealth -= ActualDamage; //take damage to monster's health
	FName HitBoneName = NAME_None;

	//if the type of attack is point like point of sword
	if(DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)&DamageEvent;
		HitBoneName = PointDamageEvent->HitInfo.BoneName;
	}
	//else if the type of attack is area damage like swing of sword
	else if(DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		FRadialDamageEvent* RadialDamageEvent = (FRadialDamageEvent*)&DamageEvent;
		if(RadialDamageEvent->ComponentHits.Num() > 0)
		{
			//get first hitted place
			HitBoneName = RadialDamageEvent->ComponentHits[0].BoneName;
		}
	}
	// 1. 맞은 본이 DestructibleParts에 없으면 부모 본을 타고 올라가며 대표 부위 찾기
	// (예: foot_l -> calf_l -> thigh_l 순서로 탐색)
	if (HitBoneName != NAME_None && GetMesh())
	{
		FName CurrentBone = HitBoneName;
		while (CurrentBone != NAME_None && !DestructibleParts.Contains(CurrentBone))
		{
			CurrentBone = GetMesh()->GetParentBone(CurrentBone);
		}

		// 상위 부모 중 DestructibleParts에 등록된 본을 찾았다면 해당 본으로 교체
		if (CurrentBone != NAME_None)
		{
			HitBoneName = CurrentBone;
		}
	}

	if(HitBoneName != NAME_None && DestructibleParts.Contains(HitBoneName))
	{
		FDestructiblePart& Part = DestructibleParts[HitBoneName];

		if(!Part.bIsDestroyed)
		{
			Part.CurrentHealth -= ActualDamage;

			if(Part.CurrentHealth <= 0.f)
			{
				Part.CurrentHealth = 0.f;
				Part.bIsDestroyed = true;

				UE_LOG(LogTemp,Warning,TEXT("part broken : %s"),*HitBoneName.ToString());

				// [추가] 부위 메시 숨기기 + 해당 부위 콜리전/피직스 비활성화
				if (GetMesh())
				{
					// PBO_Term: 해당 본과 하위 자식 본들의 피직스 바디를 종료시켜 허공에 맞는 현상 방지
					GetMesh()->HideBoneByName(HitBoneName, EPhysBodyOp::PBO_Term);
				}

				OnPartDestroyed(HitBoneName);
			}

		}
	}

	if(CurrentHealth <= 0.f)
	{
		UE_LOG(LogTemp,Warning,TEXT("monster is dead"));
	}

	return ActualDamage;

}


void AMonsterBase::PerformAttack()
{
	if (bIsAttacking || !AttackMontage)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		return;
	}

	bIsAttacking = true;
	LastAttackTime = GetWorld()->GetTimeSeconds();

	const float Duration = AnimInstance->Montage_Play(AttackMontage, 1.0f);
	if (Duration > 0.f)
	{
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &AMonsterBase::OnAttackMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
	}
	else
	{
		bIsAttacking = false;
	}
}

void AMonsterBase::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttacking = false;
}

