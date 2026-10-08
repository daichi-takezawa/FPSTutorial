#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "Weapon.h"
#include "Engine/World.h"

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

	// 武器を出現させて手に持たせる
	SpawnWeapon();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Enhanced Input用の入力コンポーネントに変換する
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!IsValid(EnhancedInputComponent))
	{
		return;
	}

	// IA_Moveが入力されている間、Move関数を呼ぶ
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

	// IA_Lookが入力されている間、Look関数を呼ぶ
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
}

void APlayerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// 操作しているのがプレイヤーのコントローラーか確認する
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!IsValid(PlayerController))
	{
		return;
	}

	// Enhanced Inputのサブシステムを取得する
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (!IsValid(Subsystem))
	{
		return;
	}

	// Input Mapping Contextを登録して、キーの割り当てを有効にする
	Subsystem->AddMappingContext(DefaultMappingContext, 0);
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// 入力された値をX(左右)とY(前後)の2つの値で受け取る
	const FVector2D MoveVector = Value.Get<FVector2D>();

	// キャラクターの正面方向に前後の入力を加える
	AddMovementInput(GetActorForwardVector(), MoveVector.Y);

	// キャラクターの右方向に左右の入力を加える
	AddMovementInput(GetActorRightVector(), MoveVector.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	// マウスの動きをX(左右)とY(上下)の2つの値で受け取る
	const FVector2D LookVector = Value.Get<FVector2D>();

	// 左右の動きでコントローラーを左右に回転させる
	AddControllerYawInput(LookVector.X);

	// 上下の動きでコントローラーを上下に回転させる
	AddControllerPitchInput(LookVector.Y);
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// キャラクターと一緒に持っている武器も消す
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->Destroy();
	}

	Super::EndPlay(EndPlayReason);
}

void APlayerCharacter::SpawnWeapon()
{
	// 武器のクラスが設定されていなければ何もしない
	if (!WeaponClass)
	{
		return;
	}

	// 出現させる武器の持ち主をこのキャラクターにする
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	// 何かと重なっていても必ず出現させる
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// キャラクターと同じ位置に武器を出現させる
	CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, GetActorTransform(), SpawnParams);
	if (!IsValid(CurrentWeapon))
	{
		return;
	}

	// 一人称用と三人称用の体のソケットに武器を取り付ける
	CurrentWeapon->AttachToOwnerMeshes(FirstPersonMesh, GetMesh(), WeaponSocketName);

	// 動作確認用のメッセージを表示する
	if (IsValid(GEngine))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Weapon Equipped"));
	}
}