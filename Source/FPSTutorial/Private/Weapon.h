#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"


// ヘッダーでは使う型だけを宣言しておく(前方宣言)
class USkeletalMeshComponent;
class USoundBase;
class UNiagaraSystem;

UCLASS()
class AWeapon : public AActor
{
	GENERATED_BODY()

public:
	// コンストラクタ
	AWeapon();

	// 武器を持ち主の体のソケットに取り付ける
	void AttachToOwnerMeshes(USkeletalMeshComponent* FirstPersonParent, USkeletalMeshComponent* ThirdPersonParent, FName SocketName);

	// 射撃を始める(ボタンを押したときに呼ぶ)
	void StartFire();

	// 射撃を止める(ボタンを離したときに呼ぶ)
	void StopFire();

protected:
	// 持ち主にだけ見える一人称用の銃のメッシュ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	// 周りから見える三人称用の銃のメッシュ(影も担当する)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> ThirdPersonMesh;

	// 1秒間に撃てる弾の数
	UPROPERTY(EditDefaultsOnly, Category = "Fire", meta = (ClampMin = "1.0"))
	float FireRate = 10.0f;

	// 弾が届く距離(cm)
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float FireRange = 10000.0f;

	// 銃口のソケットの名前
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	FName MuzzleSocketName = FName("Muzzle");

	// 射撃音
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TObjectPtr<USoundBase> FireSound;

	// 銃口の光のエフェクト
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TObjectPtr<UNiagaraSystem> MuzzleFlashEffect;

	// 弾の軌跡のエフェクト
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TObjectPtr<UNiagaraSystem> TracerEffect;

	// 着弾のエフェクト
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	// 連射に使うタイマー
	FTimerHandle FireTimerHandle;

	// 最後に撃った時間
	double LastFireTime = -1000.0;

	// 弾を1発撃つ
	void Fire();

	// 射撃音とエフェクトを出す
	void PlayFireEffects(const FVector& TracerEnd, const FHitResult& Hit, bool bHit);

	// 銃口のエフェクトを出すメッシュを選ぶ
	USkeletalMeshComponent* GetMuzzleMesh() const;

private:
	// 1発ごとに視点を上に跳ね上げる角度(度)
	UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilPitch = 0.3f;

	// 1発ごとに視点を左右にずらす最大の角度(度)
	UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilYawMax = 0.15f;

	// 撃つたびに視点を動かして反動をつける
	void ApplyRecoil(AController* OwnerController);
};