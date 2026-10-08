#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

// ヘッダーでは使う型だけを宣言しておく(前方宣言)
class ACharacter;
class UCharacterMovementComponent;

UCLASS()
class UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	// アニメーションが使われ始めたときに1回だけ呼ばれる関数
	virtual void NativeInitializeAnimation() override;

	// アニメーションの更新のたびに呼ばれる関数
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// このアニメーションを使っているキャラクター
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwnerCharacter;

	// キャラクターの移動を担当するコンポーネント
	UPROPERTY(Transient)
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	// 地面を移動している速さ(アニメーションブループリントから読む)
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed = 0.0f;
};