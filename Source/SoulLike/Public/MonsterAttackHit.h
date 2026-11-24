#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MonsterAttackHit.generated.h"

USTRUCT()
struct FMonsterAttackHitState
{
	GENERATED_BODY()

	TSet<TWeakObjectPtr<AActor>> HitActors;
	TArray<FVector> LastPoints;
};

UCLASS()
class SOULLIKE_API UMonsterAttackHit : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	// 손목/손잡이 시작 소켓 이름 (에디터 몽타주에서 변경 가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	FName StartSocket = FName("hand_r");

	// 손끝/무기 끝 소켓 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	FName EndSocket = FName("hand_r_end");

	// 판정 구체 반지름 (골렘 주먹 크기에 맞춰 조절)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float TraceRadius = 35.f;

	// 소켓 사이 보간 샘플 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	int32 SampleCount = 4;

	// 데미지 배율 (MonsterBase의 AttackDamage * DamageMultiplier)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float DamageMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawDebug = true;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	TMap<TWeakObjectPtr<USkeletalMeshComponent>, FMonsterAttackHitState> TraceStates;

	bool GetTracePoints(const USkeletalMeshComponent* MeshComp, TArray<FVector>& OutPoints) const;
};