#include "EnemyAIController.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "TimerManager.h"

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 止まる距離を最小値から最大値の間でランダムに決める(最大値が最小値より小さくならないようにする)
	StopDistance = FMath::FRandRange(MinStopDistance, FMath::Max(MinStopDistance, MaxStopDistance));
	bIsInRange = false;

	// 一定間隔でUpdateBehaviorが呼ばれるようにする
	GetWorldTimerManager().SetTimer(UpdateTimerHandle, this, &AEnemyAIController::UpdateBehavior, UpdateInterval, true);

	// 動作確認用に止まる距離をメートルで表示する
	if (IsValid(GEngine))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Enemy Stop Distance: %.1f m"), StopDistance / 100.0f));
	}
}

void AEnemyAIController::OnUnPossess()
{
	// 行動の判断を止める
	GetWorldTimerManager().ClearTimer(UpdateTimerHandle);

	Super::OnUnPossess();
}

void AEnemyAIController::UpdateBehavior()
{
	// 操作している敵と、プレイヤーのキャラクターを取得する
	APawn* ControlledPawn = GetPawn();
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	// 敵かプレイヤーがいなければ、その場で止まって待つ
	if (!IsValid(ControlledPawn) || !IsValid(PlayerPawn))
	{
		StopMovement();
		ClearFocus(EAIFocusPriority::Gameplay);
		bIsInRange = false;
		return;
	}

	// 敵とプレイヤーの水平方向の距離を計算する
	const float Distance = FVector::Dist2D(ControlledPawn->GetActorLocation(), PlayerPawn->GetActorLocation());

	// 止まっているときに、プレイヤーが余分に離れたら再び近づく
	if (bIsInRange && Distance > StopDistance + ChaseAgainDistance)
	{
		bIsInRange = false;

		// 動作確認用のメッセージを表示する
		if (IsValid(GEngine))
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Enemy Chase"));
		}
	}

	// 止まっているときは、プレイヤーの方を向き続ける
	if (bIsInRange)
	{
		SetFocus(PlayerPawn);
		return;
	}

	// まだ移動していなければ、プレイヤーに向かって歩き始める
	if (GetMoveStatus() != EPathFollowingStatus::Moving)
	{
		// 歩いている間は進む方向を向くようにする
		ClearFocus(EAIFocusPriority::Gameplay);
		MoveToActor(PlayerPawn, StopDistance, false, true, false);
	}
}

void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	// 移動に失敗したときは、次のUpdateBehaviorでもう一度歩き始める
	if (!Result.IsSuccess())
	{
		return;
	}

	// プレイヤーの近くに着いたら止まった状態にする
	bIsInRange = true;

	// すぐにプレイヤーの方を向く
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (IsValid(PlayerPawn))
	{
		SetFocus(PlayerPawn);
	}

	// 動作確認用のメッセージを表示する
	if (IsValid(GEngine))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Enemy In Range"));
	}
}