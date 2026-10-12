#include "PlayerHUDWidget.h"
#include "Components/TextBlock.h"

void UPlayerHUDWidget::SetScore(int32 NewScore)
{
	// スコアのテキストがなければ何もしない
	if (!IsValid(Score_Text))
	{
		return;
	}

	// 数字を表示用の文字に変えて、テキストに設定する
	Score_Text->SetText(FText::AsNumber(NewScore));
}