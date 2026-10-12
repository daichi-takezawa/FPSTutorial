#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnPoint.generated.h"

// ヘッダーでは使う型だけを宣言しておく(前方宣言)
class AEnemyCharacter;
class UNiagaraComponent;
class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class AEnemySpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	// コンストラクタ
	AEnemySpawnPoint();

protected:
	// ゲーム開始時に呼ばれる関数
	virtual void BeginPlay() override;

	// ポータル全体の位置を決めるためのコンポーネント
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	// ポータルの台座のメッシュ(ブループリントでメッシュを設定する)
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PadMesh;

	// ポータルのエフェクト(ブループリントでエフェクトを設定する)
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UNiagaraComponent> PortalEffect;

	// 出現させる敵のクラス(ブループリントで設定する)
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TSubclassOf<AEnemyCharacter> EnemyClass;

	// 出現の間隔の最小値(秒)
	UPROPERTY(EditDefaultsOnly, Category = "Spawn", meta = (ClampMin = "0.1"))
	float MinSpawnInterval = 1.0f;

	// 出現の間隔の最大値(秒)
	UPROPERTY(EditDefaultsOnly, Category = "Spawn", meta = (ClampMin = "0.1"))
	float MaxSpawnInterval = 7.0f;

	// ポータルの位置からどれだけ上に敵を出現させるか(cm)
	UPROPERTY(EditDefaultsOnly, Category = "Spawn", meta = (ClampMin = "0.0"))
	float SpawnHeight = 100.0f;

	// 敵を出現させるタイマー
	FTimerHandle SpawnTimerHandle;

	// タイマーで呼ばれて、敵を出現させて次のタイマーをセットする関数
	void HandleSpawnTimer();

	// ランダムな秒数で、次の出現のタイマーをセットする関数
	void ScheduleNextSpawn();

	// 敵を1体出現させる関数
	void SpawnEnemy();
};