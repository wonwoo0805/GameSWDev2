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
		FVector ToPlayer = PlayerPawn->GetActorLocation() - GetActorLocation();
		ToPlayer.Z = 0.f; // 높이 차이를 제외한 수평 방향만 계산

		FRotator TargetRot = ToPlayer.Rotation();
		TargetRot.Pitch = 0.f;
		TargetRot.Roll = 0.f;

		// 회전 속도를 조금 더 민첩하게 조절 (기존 5.0f -> 7.0f~8.0f 추천)
		FRotator SmoothRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 7.5f);
		SetActorRotation(SmoothRot);

		// 1. 거리 체크
		const float DistToPlayer = ToPlayer.Size();

		// 2. 정면 각도 체크 (몬스터 정면 벡터와 플레이어 방향 벡터의 내적)
		const FVector ForwardDir = GetActorForwardVector().GetSafeNormal2D();
		const FVector DirToPlayer = ToPlayer.GetSafeNormal();
		const float DotResult = FVector::DotProduct(ForwardDir, DirToPlayer);
		const float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(DotResult, -1.0f, 1.0f)));

		// 3. 사거리 이내 + 정면 각도 이내 + 쿨타임 경과 시에만 공격 실행
		const float CurrentTime = GetWorld()->GetTimeSeconds();
		if (DistToPlayer <= AttackRange && AngleDeg <= AttackAngleThreshold && (CurrentTime - LastAttackTime) >= AttackCooldown)
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

