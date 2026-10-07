#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/CollisionProfile.h"

APlayerCharacter::APlayerCharacter()
{
	// Tickは使わないのでオフにする
	PrimaryActorTick.bCanEverTick = false;

	// カプセルの大きさを設定する(半径、高さの半分)
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 96.0f);

	// Meshを三人称用の体として扱う(一人称用メッシュの代わりに影を落とす)
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	// Meshは自分からは見えないようにする
	GetMesh()->SetOwnerNoSee(true);

	// 一人称用メッシュを作成して、Meshの子にする
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(GetMesh());

	// 一人称用メッシュをFirst Person Renderingで描画する
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;

	// 一人称用メッシュは自分にだけ見えるようにする
	FirstPersonMesh->SetOnlyOwnerSee(true);

	// 一人称用メッシュは見た目だけなので当たり判定をなくす
	FirstPersonMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	// カメラを作成して、アニメーションで揺れないカプセルに取り付ける
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());

	// カメラを目の高さに動かす(カプセルの中心から上に64)
	FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));

	// コントローラーの向きにあわせてカメラを回転させる
	FirstPersonCamera->bUsePawnControlRotation = true;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 一人称用メッシュの頭を隠して、カメラに映り込まないようにする
	FirstPersonMesh->HideBoneByName(TEXT("head"), EPhysBodyOp::PBO_None);

	// 動作確認用のメッセージを表示する
	if (IsValid(GEngine))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("PlayerCharacter BeginPlay"));
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 移動と視点操作のバインドは9-4で追加する
}