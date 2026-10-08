#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

// ヘッダーでは使う型だけを宣言しておく(前方宣言)
class USkeletalMeshComponent;

UCLASS()
class AWeapon : public AActor
{
	GENERATED_BODY()

public:
	// コンストラクタ
	AWeapon();

	// 武器を持ち主の体のソケットに取り付ける
	void AttachToOwnerMeshes(USkeletalMeshComponent* FirstPersonParent, USkeletalMeshComponent* ThirdPersonParent, FName SocketName);

protected:
	// 持ち主にだけ見える一人称用の銃のメッシュ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	// 周りから見える三人称用の銃のメッシュ(影も担当する)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> ThirdPersonMesh;
};