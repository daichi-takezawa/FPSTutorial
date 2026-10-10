#include "HealthComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"

UHealthComponent::UHealthComponent()
{
	// 毎フレームの処理は使わないのでTickをオフにする
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ゲーム開始時はHPを満タンにする
	CurrentHealth = MaxHealth;
	bIsDead = false;

	// 持ち主がダメージを受けたら、HandleTakeAnyDamageが呼ばれるようにする
	AActor* OwnerActor = GetOwner();
	if (IsValid(OwnerActor))
	{
		OwnerActor->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// ダメージが0以下か、すでに倒れていたら何もしない
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	// HPを減らす(0より小さくならないようにする)
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);

	// HPが変わったことを知らせる
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	// HPが0になったら、倒れたことを知らせる
	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;
		OnDeath.Broadcast(InstigatedBy);
	}
}