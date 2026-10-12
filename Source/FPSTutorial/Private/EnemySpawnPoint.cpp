#include "EnemySpawnPoint.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"
#include "EnemyCharacter.h"

AEnemySpawnPoint::AEnemySpawnPoint()
{
	// Tickは使わないのでオフにする
	PrimaryActorTick.bCanEverTick = false;

	// ポータル全体の位置を決めるコンポーネントを作る
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// 台座のメッシュを作って、Rootに取り付ける
	PadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PadMesh"));
	PadMesh->SetupAttachment(Root);

	// 台座の当たり判定をなくして、見た目だけの床にする
	PadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 台座がナビメッシュに影響しないようにする
	PadMesh->SetCanEverAffectNavigation(false);

	// エフェクトを作って、Rootに取り付ける
	PortalEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalEffect"));
	PortalEffect->SetupAttachment(Root);
}

void AEnemySpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	// ランダムな秒数が経ったら、1体目を出現させる
	ScheduleNextSpawn();
}

void AEnemySpawnPoint::HandleSpawnTimer()
{
	// 敵を1体出現させる
	SpawnEnemy();

	// 次の出現のタイマーをセットする
	ScheduleNextSpawn();
}

void AEnemySpawnPoint::ScheduleNextSpawn()
{
	// 次に出現するまでの秒数をランダムに決める(最大値が最小値より小さくならないようにする)
	const float NextInterval = FMath::FRandRange(MinSpawnInterval, FMath::Max(MinSpawnInterval, MaxSpawnInterval));

	// 決めた秒数が経ったら、HandleSpawnTimerが1回呼ばれるようにする
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AEnemySpawnPoint::HandleSpawnTimer, NextInterval, false);

	// 動作確認用に次の出現までの秒数を表示する
	if (IsValid(GEngine))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("%s Next Spawn: %.1f s"), *GetActorNameOrLabel(), NextInterval));
	}
}

void AEnemySpawnPoint::SpawnEnemy()
{
	// 敵のクラスが設定されていなければ何もしない
	if (!EnemyClass)
	{
		return;
	}

	// ポータルの位置から、SpawnHeightだけ上に出現させる
	const FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, SpawnHeight);

	// 基本はポータルの左右の向きで出現させる
	FRotator SpawnRotation(0.0f, GetActorRotation().Yaw, 0.0f);

	// プレイヤーがいれば、プレイヤーの方を向いて出現させる
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (IsValid(PlayerPawn))
	{
		const FVector ToPlayer = PlayerPawn->GetActorLocation() - SpawnLocation;
		SpawnRotation = FRotator(0.0f, ToPlayer.Rotation().Yaw, 0.0f);
	}

	// 少し位置をずらしても他のものと重なるなら、出現させない設定にする
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	// 敵を出現させる
	GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
}