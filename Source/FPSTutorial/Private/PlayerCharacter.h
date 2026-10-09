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
class AWeapon;

UCLASS()
class APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// コンストラクタ
	APlayerCharacter();

	// 入力と関数を結び付ける関数
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

	// 射撃のInput Action
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction;

	// 操作するコントローラーが変わったときに呼ばれる関数
	virtual void NotifyControllerChanged() override;

	// 移動の入力を受け取る関数
	void Move(const FInputActionValue& Value);

	// 視点操作の入力を受け取る関数
	void Look(const FInputActionValue& Value);

	// 射撃のボタンが押されたときに呼ばれる関数
	void StartFire();

	// 射撃のボタンが離されたときに呼ばれる関数
	void StopFire();

	// 持たせる武器のクラス(ブループリントで設定する)
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AWeapon> WeaponClass;

	// 武器を取り付けるソケットの名前
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponSocketName = FName("WeaponSocket");

	// 今持っている武器
	UPROPERTY(VisibleInstanceOnly, Category = "Weapon")
	TObjectPtr<AWeapon> CurrentWeapon;

	// キャラクターがレベルから消えるときに呼ばれる関数
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 武器を出現させて手に持たせる関数
	void SpawnWeapon();
};