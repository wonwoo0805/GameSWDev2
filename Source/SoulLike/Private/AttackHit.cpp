#include "AttackHit.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "DrawDebugHelpers.h"
#include "../SoulLikeCharacter.h"
#include "MonsterBase.h"

// 노티파이 시작하면 맞앗던거 초기화
void UAttackHit::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	// BP 칼 궤적도 같이
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp)
	{
		return;
	}

	FAttackHitState& State = TraceStates.FindOrAdd(TWeakObjectPtr<USkeletalMeshComponent>(MeshComp));
	State.HitActors.Reset();
	State.LastPoints.Reset();
	GetBladePoints(MeshComp, State.LastPoints);
}

// 프레임별 검사해서 데미지
void UAttackHit::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp)
	{
		return;
	}

	UWorld* World = MeshComp->GetWorld();
	FAttackHitState* State = TraceStates.Find(TWeakObjectPtr<USkeletalMeshComponent>(MeshComp));
	if (!World || !State)
	{
		return;
	}

	TArray<FVector> Points;
	if (!GetBladePoints(MeshComp, Points))
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	const ASoulLikeCharacter* Player = Cast<ASoulLikeCharacter>(Owner);
	if (Player && Player->bIsDodging)
	{
		State->LastPoints = MoveTemp(Points);
		return;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AttackHit), false, Owner);
	const FCollisionShape Shape = FCollisionShape::MakeSphere(TraceRadius);
	TArray<FHitResult> TickHits;

	// 칼 맞은거 수집
	auto Sweep = [&](const FVector& From, const FVector& To)
	{
		TArray<FHitResult> Hits;
		World->SweepMultiByChannel(Hits, From, To, FQuat::Identity, TraceChannel, Shape, Params);
		TickHits.Append(Hits);

		if (bDrawDebug)
		{
			DrawDebugLine(World, From, To, Hits.Num() > 0 ? FColor::Green : FColor::Red, false, 1.f);
			for (const FHitResult& Hit : Hits)
			{
				DrawDebugSphere(World, Hit.ImpactPoint, TraceRadius, 8, FColor::Yellow, false, 1.f);
			}
		}
	};

	Sweep(Points[0], Points.Last());

	// 궤적으로 검사
	for (int32 i = 0; i < Points.Num(); ++i)
	{
		if (State->LastPoints.IsValidIndex(i))
		{
			Sweep(State->LastPoints[i], Points[i]);
		}
	}

	// 뼈있으면 뼈부터
	TickHits.StableSort([](const FHitResult& A, const FHitResult& B)
	{
		return A.BoneName != NAME_None && B.BoneName == NAME_None;
	});

	for (const FHitResult& Hit : TickHits)
	{
		ApplyHit(Owner, Player, Hit, *State);
	}

	State->LastPoints = MoveTemp(Points);
}

void UAttackHit::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	TraceStates.Remove(TWeakObjectPtr<USkeletalMeshComponent>(MeshComp));
}

FString UAttackHit::GetNotifyName_Implementation() const
{
	return TEXT("Attack Hit");
}

bool UAttackHit::GetBladePoints(const USkeletalMeshComponent* MeshComp, TArray<FVector>& OutPoints) const
{

	if (!MeshComp->DoesSocketExist(StartSocket) || !MeshComp->DoesSocketExist(EndSocket))
	{
		return false;
	}

	const FVector Start = MeshComp->GetSocketLocation(StartSocket);
	const FVector End = MeshComp->GetSocketLocation(EndSocket);
	const int32 Count = FMath::Max(SampleCount, 2);

	OutPoints.SetNum(Count);
	for (int32 i = 0; i < Count; ++i)
	{
		OutPoints[i] = FMath::Lerp(Start, End, static_cast<double>(i) / (Count - 1));
	}

	return true;
}

// 몬스터면 데미지
void UAttackHit::ApplyHit(AActor* Owner, const ASoulLikeCharacter* Player, const FHitResult& Hit, FAttackHitState& State) const
{
	// 아님말고
	AMonsterBase* Monster = Cast<AMonsterBase>(Hit.GetActor());
	if (!Monster || Monster->CurrentHealth <= 0.f)
	{
		return;
	}

	// 맞은놈 또때리기 금지
	const TWeakObjectPtr<AActor> MonsterKey(Monster);
	if (State.HitActors.Contains(MonsterKey))
	{
		return;
	}
	State.HitActors.Add(MonsterKey);

	// Attack * 배율로 데미지
	if (!Player)
	{
		return;
	}
	const float Damage = Player->Attack * DamageMultiplier;
	const FVector Direction = (Hit.TraceEnd - Hit.TraceStart).GetSafeNormal();
	AController* InstigatorController = Owner ? Owner->GetInstigatorController() : nullptr;

	// 데미지 전달
	UGameplayStatics::ApplyPointDamage(Monster, Damage, Direction, Hit, InstigatorController, Owner, UDamageType::StaticClass());
}
