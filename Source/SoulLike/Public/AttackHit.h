#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AttackHit.generated.h"

class ASoulLikeCharacter;

// 휘두르기 한 번 동안 유지하는 판정 상태
struct FAttackHitState
{
	// 지난 프레임의 칼날 위 점 위치
	TArray<FVector> LastPoints;

	// 이번 휘두르기에서 이미 맞은 적 (중복 타격 방지)
	TSet<TWeakObjectPtr<AActor>> HitActors;
};

UCLASS(meta = (DisplayName = "Attack Hit"))
class SOULLIKE_API UAttackHit : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	// 소켓끼리 연결해서 그거만큼 판정내는거
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
	FName StartSocket = TEXT("SwordStart");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
	FName EndSocket = TEXT("SwordEnd");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (ClampMin = "2", ClampMax = "16"))
	int32 SampleCount = 5;

	// 크기 키우면 판정 더 굵어짐
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (ClampMin = "0.0"))
	float TraceRadius = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	// 최종데미지 곱해서 계산함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0.0"))
	float DamageMultiplier = 1.f;

	// 테스트용도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bDrawDebug = false;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	TMap<TWeakObjectPtr<USkeletalMeshComponent>, FAttackHitState> TraceStates;

	bool GetBladePoints(const USkeletalMeshComponent* MeshComp, TArray<FVector>& OutPoints) const;
	void ApplyHit(AActor* Owner, const ASoulLikeCharacter* Player, const FHitResult& Hit, FAttackHitState& State) const;
};
