#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

// ヘッダーでは使う型だけを宣言しておく(前方宣言)
class UTextBlock;
class UProgressBar;
class UOverlay;
class UWidgetAnimation;

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

	// GAME OVERをアニメーション付きで表示する
	void ShowGameOver();

protected:
	// スコアの数字を表示するテキスト(WBPにある同じ名前のテキストと自動でつながる)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Score_Text;

	// HPを表示するプログレスバー(WBPにある同じ名前のプログレスバーと自動でつながる)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> Health_ProgressBar;

	// GAME OVERのUI全体をまとめたオーバーレイ(WBPにある同じ名前のオーバーレイと自動でつながる)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> GameOverBox_Overlay;

	// GAME OVERを表示するアニメーション(WBPにある同じ名前のアニメーションと自動でつながる)
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> GameOverAnim;
};