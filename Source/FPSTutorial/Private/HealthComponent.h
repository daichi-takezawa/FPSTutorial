#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

// ヘッダーでは使う型だけを宣言しておく(前方宣言)
class AController;
class UDamageType;

// HPが変わったことを知らせるデリゲート(今のHPと最大HPを渡す)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, NewHealth, float, MaxHealth);

// HPが0になったことを知らせるデリゲート(倒した相手のコントローラーを渡す)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, AController*, Killer);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// コンストラクタ
	UHealthComponent();

	// HPが変わったときに呼ばれるデリゲート
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChangedSignature OnHealthChanged;

	// HPが0になったときに呼ばれるデリゲート
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathSignature OnDeath;

	// 今のHPを返す
	float GetHealth() const { return CurrentHealth; }

	// 最大HPを返す
	float GetMaxHealth() const { return MaxHealth; }

	// 倒れているかどうかを返す
	bool IsDead() const { return bIsDead; }

protected:
	// ゲーム開始時に呼ばれる関数
	virtual void BeginPlay() override;

	// 最大HP(ブループリントで変更できる)
	UPROPERTY(EditDefaultsOnly, Category = "Health", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	// 今のHP(ゲーム中に詳細パネルで確認できる)
	UPROPERTY(VisibleInstanceOnly, Category = "Health")
	float CurrentHealth = 0.0f;

	// 倒れているかどうか
	bool bIsDead = false;

	// 持ち主がダメージを受けたときに呼ばれる関数
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
};