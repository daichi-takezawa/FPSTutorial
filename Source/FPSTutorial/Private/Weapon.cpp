#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/CollisionProfile.h"

AWeapon::AWeapon()
{
	// Tickは使わないのでオフにする
	PrimaryActorTick.bCanEverTick = false;

	// 三人称用の銃のメッシュを作成して、アクタのルートにする
	ThirdPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonMesh"));
	SetRootComponent(ThirdPersonMesh);

	// 三人称用の銃を、一人称用の銃の代わりに影を落とすメッシュとして扱う
	ThirdPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	// 三人称用の銃は持ち主からは見えないようにする
	ThirdPersonMesh->SetOwnerNoSee(true);

	// 銃は見た目だけなので当たり判定をなくす
	ThirdPersonMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	// 一人称用の銃のメッシュを作成する
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(ThirdPersonMesh);

	// 一人称用の銃をFirst Person Renderingで描画する
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;

	// 一人称用の銃は持ち主にだけ見えるようにする
	FirstPersonMesh->SetOnlyOwnerSee(true);

	// 銃は見た目だけなので当たり判定をなくす
	FirstPersonMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

void AWeapon::AttachToOwnerMeshes(USkeletalMeshComponent* FirstPersonParent, USkeletalMeshComponent* ThirdPersonParent, FName SocketName)
{
	// ソケットの位置と向きにぴったり合わせる(大きさは銃のまま)
	const FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;

	// 三人称用の銃を三人称用の体のソケットに取り付ける
	if (IsValid(ThirdPersonParent))
	{
		ThirdPersonMesh->AttachToComponent(ThirdPersonParent, AttachRules, SocketName);
	}

	// 一人称用の体がない持ち主(敵など)では一人称用の銃を使わない
	if (!IsValid(FirstPersonParent))
	{
		FirstPersonMesh->SetHiddenInGame(true);
		return;
	}

	// 一人称用の銃を一人称用の体のソケットに取り付ける
	FirstPersonMesh->AttachToComponent(FirstPersonParent, AttachRules, SocketName);
}