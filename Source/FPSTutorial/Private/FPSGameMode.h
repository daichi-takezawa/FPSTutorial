#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSGameMode.generated.h"

UCLASS()
class AFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// スコアを加算して、画面の表示を更新する
	void AddScore(int32 Amount);

protected:
	// 今のスコア(ゲーム中に詳細パネルで確認できる)
	UPROPERTY(VisibleInstanceOnly, Category = "Score")
	int32 Score = 0;
};