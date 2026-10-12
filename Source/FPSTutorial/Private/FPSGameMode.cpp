#include "FPSGameMode.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

void AFPSGameMode::AddScore(int32 Amount)
{
	// 加算する値が0以下なら何もしない
	if (Amount <= 0)
	{
		return;
	}

	// スコアを加算する
	Score += Amount;

	// 動作確認用に今のスコアを表示する
	if (IsValid(GEngine))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Score: %d"), Score));
	}

	// プレイヤーのキャラクターを取得する
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	// プレイヤーに画面のスコアを更新してもらう
	PlayerCharacter->UpdateScore(Score);
}