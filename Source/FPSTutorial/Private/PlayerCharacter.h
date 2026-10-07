#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"


// ヘッダーでは使う型だけを宣言しておく(前方宣言)
class UCameraComponent;
class USkeletalMeshComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

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

	// プレイヤーの操作をまとめたInput Mapping Context
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// 移動のInput Action
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	// 視点操作のInput Action
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	// 操作するコントローラーが変わったときに呼ばれる関数
	virtual void NotifyControllerChanged() override;

	// 移動の入力を受け取る関数
	void Move(const FInputActionValue& Value);

	// 視点操作の入力を受け取る関数
	void Look(const FInputActionValue& Value);
};