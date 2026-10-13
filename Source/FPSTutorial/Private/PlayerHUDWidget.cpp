#include "PlayerHUDWidget.h"
#include "Components/TextBlock.h"

// ここから追加
#include "Components/ProgressBar.h"

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

// ここから追加
void UPlayerHUDWidget::SetHealth(float CurrentHealth, float MaxHealth)
{
	// HPバーがなければ何もしない
	if (!IsValid(Health_ProgressBar))
	{
		return;
	}

	// 最大HPが0以下だと割り算ができないので何もしない
	if (MaxHealth <= 0.0f)
	{
		return;
	}

	// 今のHPが最大HPの何割かを計算する(0.0から1.0の範囲に収める)
	const float HealthPercent = FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f);

	// 計算した割合をHPバーに設定する
	Health_ProgressBar->SetPercent(HealthPercent);
}