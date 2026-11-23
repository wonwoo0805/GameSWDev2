#include "MonsterAttackHit.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "DrawDebugHelpers.h"
#include "MonsterBase.h"
#include "../SoulLikeCharacter.h"

void UMonsterAttackHit::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp)
	{
		return;
	}

	FMonsterAttackHitState& State = TraceStates.FindOrAdd(TWeakObjectPtr<USkeletalMeshComponent>(MeshComp));
	State.HitActors.Reset();
	State.LastPoints.Reset();
	GetTracePoints(MeshComp, State.LastPoints);
}

void UMonsterAttackHit::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp)
	{
		return;
	}

	UWorld* World = MeshComp->GetWorld();
	FMonsterAttackHitState* State = TraceStates.Find(TWeakObjectPtr<USkeletalMeshComponent>(MeshComp));
	if (!World || !State)
	{
		return;
	}

	TArray<FVector> Points;
	if (!GetTracePoints(MeshComp, Points))
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	const AMonsterBase* Monster = Cast<AMonsterBase>(Owner);
	if (!Monster || Monster->CurrentHealth <= 0.f)
	{
		return;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(MonsterAttackHit), false, Owner);
	const FCollisionShape Shape = FCollisionShape::MakeSphere(TraceRadius);
	TArray<FHitResult> TickHits;

	auto Sweep = [&](const FVector& From, const FVector& To)
	{
		TArray<FHitResult> Hits;
		World->SweepMultiByChannel(Hits, From, To, FQuat::Identity, TraceChannel, Shape, Params);
		TickHits.Append(Hits);

		if (bDrawDebug)
		{
			DrawDebugLine(World, From, To, Hits.Num() > 0 ? FColor::Green : FColor::Red, false, 1.0f);
			for (const FHitResult& Hit : Hits)
			{
				DrawDebugSphere(World, Hit.ImpactPoint, TraceRadius, 8, FColor::Yellow, false, 1.0f);
			}
		}
	};

	// 현재 프레임 손/무기 길이 방향 스윕
	Sweep(Points[0], Points.Last());

	// 이전 프레임 -> 현재 프레임 궤적 스윕
	for (int32 i = 0; i < Points.Num(); ++i)
	{
		if (State->LastPoints.IsValidIndex(i))
		{
			Sweep(State->LastPoints[i], Points[i]);
		}
	}

	for (const FHitResult& Hit : TickHits)
	{
		ASoulLikeCharacter* HitPlayer = Cast<ASoulLikeCharacter>(Hit.GetActor());
		if (!HitPlayer)
		{
			continue;
		}

		// 한 번의 공격 노티파이 구간 동안 플레이어 중복 피격 방지
		const TWeakObjectPtr<AActor> PlayerKey(HitPlayer);
		if (State->HitActors.Contains(PlayerKey))
		{
			continue;
		}
		State->HitActors.Add(PlayerKey);

		const float FinalDamage = Monster->AttackDamage * DamageMultiplier;
		const FVector HitDir = (Hit.TraceEnd - Hit.TraceStart).GetSafeNormal();
		AController* InstigatorCtrl = Owner ? Owner->GetInstigatorController() : nullptr;

		UGameplayStatics::ApplyPointDamage(HitPlayer, FinalDamage, HitDir, Hit, InstigatorCtrl, Owner, UDamageType::StaticClass());
		UE_LOG(LogTemp, Warning, TEXT("Monster Hit Player! Damage: %f"), FinalDamage);
	}

	State->LastPoints = MoveTemp(Points);
}

void UMonsterAttackHit::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	TraceStates.Remove(TWeakObjectPtr<USkeletalMeshComponent>(MeshComp));
}

FString UMonsterAttackHit::GetNotifyName_Implementation() const
{
	return TEXT("Monster Attack Hit");
}

bool UMonsterAttackHit::GetTracePoints(const USkeletalMeshComponent* MeshComp, TArray<FVector>& OutPoints) const
{
	// 끝 소켓이 없으면 시작 소켓 한 점만으로 트레이스하도록 허용 (단순 주먹 판정 편의성)
	if (!MeshComp->DoesSocketExist(StartSocket))
	{
		return false;
	}

	const FVector Start = MeshComp->GetSocketLocation(StartSocket);
	const FVector End = MeshComp->DoesSocketExist(EndSocket) ? MeshComp->GetSocketLocation(EndSocket) : Start;
	const int32 Count = FMath::Max(SampleCount, 2);

	OutPoints.SetNum(Count);
	for (int32 i = 0; i < Count; ++i)
	{
		OutPoints[i] = FMath::Lerp(Start, End, static_cast<double>(i) / (Count - 1));
	}

	return true;
}