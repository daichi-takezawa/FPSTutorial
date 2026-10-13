#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

// ヘッダーでは使う型だけを宣言しておく(前方宣言)
class UTextBlock;

// ここから追加
class UProgressBar;

UCLASS()
class UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// スコアの表示を更新する
	void SetScore(int32 NewScore);

	// ここから追加
	// HPバーの表示を更新する(今のHPと最大HPを受け取る)
	void SetHealth(float CurrentHealth, float MaxHealth);

protected:
	// スコアの数字を表示するテキスト(WBPにある同じ名前のテキストと自動でつながる)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Score_Text;

	// ここから追加
	// HPを表示するプログレスバー(WBPにある同じ名前のプログレスバーと自動でつながる)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> Health_ProgressBar;
};