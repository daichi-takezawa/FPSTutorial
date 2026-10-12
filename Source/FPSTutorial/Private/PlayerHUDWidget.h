#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

// ヘッダーでは使う型だけを宣言しておく(前方宣言)
class UTextBlock;

UCLASS()
class UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// スコアの表示を更新する
	void SetScore(int32 NewScore);

protected:
	// スコアの数字を表示するテキスト(WBPにある同じ名前のテキストと自動でつながる)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Score_Text;
};