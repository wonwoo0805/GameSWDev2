// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterBase.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "BossHUDWidget.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AMonsterBase::AMonsterBase()
{
	MaxHealth = 200.f;
	CurrentHealth = MaxHealth;
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 레벨에 배치되거나 스폰될 때 자동으로 AIController가 빙의되도록 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	}

}

// Called when the game starts or when spawned
void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	
}

// Called every frame
void AMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentHealth <= 0.f || bIsAttacking)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		return;
	}

	FVector ToPlayer = PlayerPawn->GetActorLocation() - GetActorLocation();
	ToPlayer.Z = 0.f;
	const float DistToPlayer = ToPlayer.Size();

	// 1. 발견 범위 밖이면 무시
	if (DistToPlayer > DetectRange)
	{
		return;
	}

	// 2. 발견 범위 안: 시선(회전) 따라가기
	const FVector DirToPlayer = ToPlayer.GetSafeNormal();
	FRotator TargetRot = DirToPlayer.Rotation();
	TargetRot.Pitch = 0.f;
	TargetRot.Roll = 0.f;

	FRotator SmoothRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 7.5f);
	SetActorRotation(SmoothRot);

	// 3. 공격 범위 안: 정면 각도 + 쿨타임 체크 후 공격
	if (DistToPlayer <= AttackRange)
	{
		const FVector ForwardDir = GetActorForwardVector().GetSafeNormal2D();
		const float DotResult = FVector::DotProduct(ForwardDir, DirToPlayer);
		const float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(DotResult, -1.0f, 1.0f)));

		const float CurrentTime = GetWorld()->GetTimeSeconds();
		if (AngleDeg <= AttackAngleThreshold && (CurrentTime - LastAttackTime) >= AttackCooldown)
		{
			PerformAttack();
		}
		return;
	}

	// 4. 추적 범위 안 (AttackRange < 거리 <= ChaseRange): 플레이어 쪽으로 이동
	if (DistToPlayer <= ChaseRange)
	{
		ShowBossHUD();
		AddMovementInput(DirToPlayer, 1.0f);
	}
	else {
		HideBossHUD();
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

	//update boss' health bar
	if (BossHUDWidget != nullptr)
	{
		BossHUDWidget->UpdateBossHP(CurrentHealth, MaxHealth);
	}

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

				if (GetMesh())
				{
					GetMesh()->HideBoneByName(HitBoneName, EPhysBodyOp::PBO_Term);

					// 파괴된 본(또는 그 하위 본)의 소켓에 붙어 있는 무기 컴포넌트 찾기
					TArray<USceneComponent*> AttachedChildren = GetMesh()->GetAttachChildren();
					for (USceneComponent* Child : AttachedChildren)
					{
						if (!Child)
						{
							continue;
						}

						// 자식 컴포넌트가 붙어 있는 소켓(예: weapon_r)의 실제 부모 본 이름 가져오기
						const FName AttachedSocket = Child->GetAttachSocketName();
						const FName SocketBone = GetMesh()->GetSocketBoneName(AttachedSocket);

						// 파괴된 본 자신이거나 그 하위 본에 붙어 있는 무기라면 처리
						if (SocketBone == HitBoneName || GetMesh()->BoneIsChildOf(SocketBone, HitBoneName))
						{
							// [옵션 1] 깔끔하게 즉시 숨기기
							Child->SetVisibility(false, true);
							if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Child))
							{
								PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
							}

							/* [옵션 2] 숨기는 대신 바닥으로 툭 떨어뜨리기 (연출용, 원하면 옵션 1 대신 사용)
							Child->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
							if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Child))
							{
								PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
								PrimComp->SetSimulatePhysics(true);
							}
							*/
						}
					}
				}

				OnPartDestroyed(HitBoneName);
			}

		}
	}

	if (CurrentHealth <= 0.f && !bIsDead)
	{
		Die();
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

void AMonsterBase::Die()
{
	if (bIsDead)
	{
		return;
	}
	bIsDead = true;
	bIsAttacking = false;
	HideBossHUD();

	UE_LOG(LogTemp, Warning, TEXT("Monster Died!"));

	// 1. 진행 중인 공격 몽타주 및 이동 정지
	if (UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
	{
		AnimInstance->StopAllMontages(0.2f);
	}
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}

	// 2. 캡슐 콜리전 비활성화 (죽은 몬스터에 플레이어가 걸리지 않도록)
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 3. 래그돌 켜기 전에 부착된 자식 컴포넌트(무기 등) 콜리전 비활성화
	if (GetMesh())
	{
		TArray<USceneComponent*> AttachedChildren = GetMesh()->GetAttachChildren();
		for (USceneComponent* Child : AttachedChildren)
		{
			if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Child))
			{
				PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}

		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
	}

	// 3. 사망 연출 (사망 애니메이션이 없다면 래그돌로 바닥에 쓰러뜨리기)
	if (GetMesh())
	{
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetSimulatePhysics(true);
	}

	// 4. 소재/아이템 드롭 스폰
	if (DropItemClass && GetWorld())
	{
		FVector SpawnLoc = GetActorLocation() + FVector(0.f, 0.f, 50.f);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		GetWorld()->SpawnActor<AActor>(DropItemClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);
	}

	// 5. 블루프린트 이벤트 호출 (드롭 테이블, 이펙트, 사운드 확장용)
	OnMonsterDied();

	// 6. 5초 뒤 자동으로 Destroy() 호출되도록 수명 설정
	SetLifeSpan(5.0f);
}

void AMonsterBase::ShowBossHUD()
{
	if (!bIsBoss || bIsDead || BossHUDClass == nullptr) return;

	// create bossHUD
	if (BossHUDWidget == nullptr)
	{
		BossHUDWidget = CreateWidget<UBossHUDWidget>(GetWorld(), BossHUDClass);
	}

	//fix HUD on ViewPort
	if (BossHUDWidget != nullptr && !BossHUDWidget->IsInViewport())
	{
		BossHUDWidget->InitBossInfo(BossName, CurrentHealth, MaxHealth);
		BossHUDWidget->AddToViewport(5);
	}
}

void AMonsterBase::HideBossHUD()
{
	//remove bossHUD
	if (BossHUDWidget != nullptr && BossHUDWidget->IsInViewport())
	{
		BossHUDWidget->RemoveFromParent();
	}
}

