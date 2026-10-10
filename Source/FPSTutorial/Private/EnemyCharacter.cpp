#include "EnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Weapon.h"
#include "HealthComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
	// Tickは使わないのでオフにする
	PrimaryActorTick.bCanEverTick = false;

	// カプセルの大きさをプレイヤーと同じにする(半径、高さの半分)
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 96.0f);

	// 敵の歩く速さをプレイヤーより少し遅くする
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	// HPを管理するコンポーネントを作成する
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 武器を出現させて手に持たせる
	SpawnWeapon();

	// HPが変わったときと0になったときに、自分の関数が呼ばれるようにする
	if (IsValid(HealthComponent))
	{
		HealthComponent->OnHealthChanged.AddDynamic(this, &AEnemyCharacter::HandleHealthChanged);
		HealthComponent->OnDeath.AddDynamic(this, &AEnemyCharacter::HandleDeath);
	}
}

void AEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// キャラクターと一緒に持っている武器も消す
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->Destroy();
	}

	Super::EndPlay(EndPlayReason);
}

void AEnemyCharacter::SpawnWeapon()
{
	// 武器のクラスが設定されていなければ何もしない
	if (!WeaponClass)
	{
		return;
	}

	// 出現させる武器の持ち主をこの敵にする
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	// 何かと重なっていても必ず出現させる
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 敵と同じ位置に武器を出現させる
	CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, GetActorTransform(), SpawnParams);
	if (!IsValid(CurrentWeapon))
	{
		return;
	}

	// 敵には一人称用の体がないので、三人称用の体だけに取り付ける
	CurrentWeapon->AttachToOwnerMeshes(nullptr, GetMesh(), WeaponSocketName);

	// 動作確認用のメッセージを表示する
	if (IsValid(GEngine))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Enemy Weapon Equipped"));
	}
}

void AEnemyCharacter::HandleHealthChanged(float NewHealth, float MaxHealth)
{
	// 動作確認用に今のHPを表示する
	if (IsValid(GEngine))
	{
		GEngine->AddOnScreenDebugMessage(2, 2.0f, FColor::Green, FString::Printf(TEXT("Enemy HP: %.0f / %.0f"), NewHealth, MaxHealth));
	}
}

void AEnemyCharacter::HandleDeath(AController* Killer)
{
	// 動作確認用に倒れたことを表示する
	if (IsValid(GEngine))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Enemy Dead"));
	}
}