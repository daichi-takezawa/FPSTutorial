#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

// ヘッダーでは使う型だけを宣言しておく(前方宣言)
class UCameraComponent;
class USkeletalMeshComponent;

UCLASS()
class APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// コンストラクタ
	APlayerCharacter();

	// 入力のバインドを行う関数(中身は9-4で書く)
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	// ゲーム開始時に呼ばれる関数
	virtual void BeginPlay() override;

	// 一人称視点のカメラ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	// 自分にだけ見える一人称用のメッシュ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;
};