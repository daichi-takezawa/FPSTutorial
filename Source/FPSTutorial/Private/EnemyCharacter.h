#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

// ヘッダーでは使う型だけを宣言しておく(前方宣言)
class AWeapon;

UCLASS()
class AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// コンストラクタ
	AEnemyCharacter();

protected:
	// ゲーム開始時に呼ばれる関数
	virtual void BeginPlay() override;

	// キャラクターがレベルから消えるときに呼ばれる関数
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 持たせる武器のクラス(ブループリントで設定する)
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AWeapon> WeaponClass;

	// 武器を取り付けるソケットの名前
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponSocketName = FName("WeaponSocket");

	// 今持っている武器
	UPROPERTY(VisibleInstanceOnly, Category = "Weapon")
	TObjectPtr<AWeapon> CurrentWeapon;

	// 武器を出現させて手に持たせる関数
	void SpawnWeapon();
};