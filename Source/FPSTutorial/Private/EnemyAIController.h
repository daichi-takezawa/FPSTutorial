#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

UCLASS()
class AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	// 移動が終わったときに呼ばれる関数
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

protected:
	// キャラクターを操作し始めたときに呼ばれる関数
	virtual void OnPossess(APawn* InPawn) override;

	// キャラクターの操作をやめたときに呼ばれる関数
	virtual void OnUnPossess() override;

	// 止まる距離の最小値(cm)
	UPROPERTY(EditDefaultsOnly, Category = "AI", meta = (ClampMin = "0.0"))
	float MinStopDistance = 600.0f;

	// 止まる距離の最大値(cm)
	UPROPERTY(EditDefaultsOnly, Category = "AI", meta = (ClampMin = "0.0"))
	float MaxStopDistance = 1000.0f;

	// 止まる距離よりこの距離だけ余分に離れたら、再び近づく(cm)
	UPROPERTY(EditDefaultsOnly, Category = "AI", meta = (ClampMin = "0.0"))
	float ChaseAgainDistance = 200.0f;

	// 行動を判断する間隔(秒)
	UPROPERTY(EditDefaultsOnly, Category = "AI", meta = (ClampMin = "0.05"))
	float UpdateInterval = 0.2f;

	// この敵が止まる距離(ゲーム開始時にランダムで決まる)
	UPROPERTY(VisibleInstanceOnly, Category = "AI")
	float StopDistance = 0.0f;

	// プレイヤーの近くで止まっているかどうか
	UPROPERTY(VisibleInstanceOnly, Category = "AI")
	bool bIsInRange = false;

	// 行動を判断するタイマー
	FTimerHandle UpdateTimerHandle;

	// 一定間隔で呼ばれて、次の行動を判断する関数
	void UpdateBehavior();
};