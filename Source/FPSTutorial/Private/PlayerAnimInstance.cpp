#include "PlayerAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// このアニメーションを使っているキャラクターを取得する
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (!IsValid(OwnerCharacter))
	{
		return;
	}

	// 毎回探さなくて済むように、移動のコンポーネントを覚えておく
	MovementComponent = OwnerCharacter->GetCharacterMovement();
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// エディタのプレビュー中などはキャラクターがいないので何もしない
	if (!IsValid(MovementComponent))
	{
		return;
	}

	// 速度から上下の動きを除いて、地面を移動している速さを計算する
	GroundSpeed = static_cast<float>(MovementComponent->Velocity.Size2D());

	// 視点の上下の角度を-180から180の範囲に直して取得する
	AimPitch = static_cast<float>(FRotator::NormalizeAxis(OwnerCharacter->GetBaseAimRotation().Pitch));
}