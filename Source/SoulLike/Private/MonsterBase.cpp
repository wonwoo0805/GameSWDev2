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

    // 1. 월드에 있는 첫 번째 플레이어(로컬/호스트) 폰 가져오기
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    // 플레이어가 살아있고 유효할 때만 실행
    if (PlayerPawn)
    {
        // 2. 내 위치에서 플레이어 위치를 바라보는 목표 회전값(Rotator) 계산
        FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerPawn->GetActorLocation());

        // 3. 몬스터가 하늘을 보거나 땅으로 처박히지 않도록 좌우(Yaw) 회전만 남기기
        TargetRot.Pitch = 0.f;
        TargetRot.Roll = 0.f;

        // 4. 확 돌아보는 게 아니라 부드럽게 휙~ 쳐다보도록 보간(Interpolation) 적용
        // 마지막 숫자인 5.0f를 올리면 홱홱 돌아보고, 낮추면 묵직하게 돌아보지요
        FRotator SmoothRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 5.0f);

        // 5. 몬스터 본체 회전 적용
        SetActorRotation(SmoothRot);
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
			}
		}
	}

	if(CurrentHealth <= 0.f)
	{
		UE_LOG(LogTemp,Warning,TEXT("monster is dead"));
	}

	return ActualDamage;

}

