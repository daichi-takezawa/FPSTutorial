#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Engine/Engine.h"
#include "GameFramework/DamageType.h"


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

void AWeapon::StartFire()
{
	// 1発ごとの間隔(秒)を計算する
	const float FireInterval = 1.0f / FireRate;

	// 前に撃ってから間隔の時間が経っていなければ、残りの時間だけ待つ
	const double Now = GetWorld()->GetTimeSeconds();
	const float FirstDelay = FMath::Max(0.0f, static_cast<float>(LastFireTime + FireInterval - Now));

	if (FirstDelay <= 0.0f)
	{
		// すぐに1発目を撃つ
		Fire();

		// 2発目からは間隔ごとに撃ち続ける
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AWeapon::Fire, FireInterval, true);
	}
	else
	{
		// 残りの時間が経ってから撃ち始める
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AWeapon::Fire, FireInterval, true, FirstDelay);
	}
}

void AWeapon::StopFire()
{
	// 連射のタイマーを止める
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void AWeapon::Fire()
{
	// 撃った時間を記録する
	LastFireTime = GetWorld()->GetTimeSeconds();

	// 持ち主のキャラクターがいなければ撃たない
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!IsValid(OwnerPawn))
	{
		StopFire();
		return;
	}

	// 持ち主を操作しているコントローラーがいなければ撃たない
	AController* OwnerController = OwnerPawn->GetController();
	if (!IsValid(OwnerController))
	{
		StopFire();
		return;
	}

	// 持ち主の視点の位置と向きを取得する
	FVector ViewLocation;
	FRotator ViewRotation;
	OwnerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	// 撃つたびに視点を跳ね上げる
	ApplyRecoil(OwnerController);

	// 視点から見ている方向に線を伸ばす
	const FVector TraceStart = ViewLocation;
	const FVector TraceEnd = TraceStart + ViewRotation.Vector() * FireRange;

	// 武器自身と持ち主には当たらないようにする
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(WeaponFire), true);
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(OwnerPawn);

	// 武器用のトレースチャンネル(プロジェクト設定で追加したWeapon)で、最初に当たったものを調べる
	FHitResult Hit;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_GameTraceChannel1, QueryParams);

	// 弾の軌跡の終わりは、当たった位置か線の終わり
	const FVector TracerEnd = bHit ? Hit.ImpactPoint : TraceEnd;

	// 射撃音とエフェクトを出す
	PlayFireEffects(TracerEnd, Hit, bHit);

	// 何かに当たったら、当たったアクタにダメージを与える
	if (bHit && IsValid(Hit.GetActor()))
	{
		UGameplayStatics::ApplyDamage(Hit.GetActor(), Damage, OwnerController, this, UDamageType::StaticClass());
	}

	// 動作確認用に当たったアクタの名前を表示する
	if (bHit && IsValid(Hit.GetActor()) && IsValid(GEngine))
	{
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Green, FString::Printf(TEXT("Hit: %s"), *Hit.GetActor()->GetName()));
	}
}

void AWeapon::PlayFireEffects(const FVector& TracerEnd, const FHitResult& Hit, bool bHit)
{
	// 銃口のエフェクトを出すメッシュを選ぶ
	USkeletalMeshComponent* MuzzleMesh = GetMuzzleMesh();
	if (!IsValid(MuzzleMesh))
	{
		return;
	}

	// 銃口の位置を取得する
	const FVector MuzzleLocation = MuzzleMesh->GetSocketLocation(MuzzleSocketName);

	// 弾が飛んだ向きを計算する
	const FVector FireDirection = (TracerEnd - MuzzleLocation).GetSafeNormal();

	// 銃口の位置で射撃音を鳴らす
	if (IsValid(FireSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, MuzzleLocation);
	}

	// 銃口の光を銃口のソケットに取り付けて出す
	if (IsValid(MuzzleFlashEffect))
	{
		UNiagaraComponent* MuzzleFlashComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFlashEffect, MuzzleMesh, MuzzleSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);

		if (IsValid(MuzzleFlashComponent))
		{
			// 煙や火花が飛ぶ向きを、弾が飛んだ向きにする
			MuzzleFlashComponent->SetVariableVec3(FName("User.Direction"), FireDirection);

			// Triggerをオンにして再生する
			MuzzleFlashComponent->SetVariableBool(FName("User.Trigger"), true);
		}
	}

	// 当たった位置のリスト(1発なので1つだけ)
	TArray<FVector> ImpactPositions;
	ImpactPositions.Add(TracerEnd);

	// 銃口から当たった位置に向けて弾の軌跡を出す
	if (IsValid(TracerEffect))
	{
		UNiagaraComponent* TracerComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, TracerEffect, MuzzleLocation, FireDirection.Rotation());

		if (IsValid(TracerComponent))
		{
			// 軌跡の始まりの位置を渡す
			TracerComponent->SetVariableVec3(FName("User.MuzzlePosition"), MuzzleLocation);

			// 軌跡の終わりの位置を渡す
			UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(TracerComponent, FName("User.ImpactPositions"), ImpactPositions);

			// Triggerをオンにして再生する
			TracerComponent->SetVariableBool(FName("User.Trigger"), true);
		}
	}

	// 何かに当たったら、当たった位置に着弾のエフェクトを出す
	if (bHit && IsValid(ImpactEffect))
	{
		UNiagaraComponent* ImpactComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());

		if (IsValid(ImpactComponent))
		{
			// 当たった面の向きのリスト(1発なので1つだけ)
			TArray<FVector> ImpactNormals;
			ImpactNormals.Add(Hit.ImpactNormal);

			// 当たった位置と面の向きと数を渡す
			UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(ImpactComponent, FName("User.ImpactPositions"), ImpactPositions);
			UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(ImpactComponent, FName("User.ImpactNormals"), ImpactNormals);
			ImpactComponent->SetVariableInt(FName("User.NumberOfHits"), 1);

			// 弾を撃った位置を渡す
			ImpactComponent->SetVariableVec3(FName("User.MuzzlePosition"), MuzzleLocation);
		}
	}
}

USkeletalMeshComponent* AWeapon::GetMuzzleMesh() const
{
	// プレイヤーが操作している武器なら、画面に映っている一人称用の銃を使う
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (IsValid(OwnerPawn) && OwnerPawn->IsPlayerControlled() && OwnerPawn->IsLocallyControlled())
	{
		return FirstPersonMesh;
	}

	// それ以外(敵など)は三人称用の銃を使う
	return ThirdPersonMesh;
}

void AWeapon::ApplyRecoil(AController* OwnerController)
{
	// プレイヤーが持っているときだけ反動をつける
	if (!OwnerController->IsPlayerController())
	{
		return;
	}

	// 左右の反動はランダムに決める
	const float RecoilYaw = FMath::FRandRange(-RecoilYawMax, RecoilYawMax);

	// 今の視点の向きに反動の角度を足す
	FRotator NewRotation = OwnerController->GetControlRotation();
	NewRotation.Pitch += RecoilPitch;
	NewRotation.Yaw += RecoilYaw;

	// 反動を足した向きを視点に反映する
	OwnerController->SetControlRotation(NewRotation);
}