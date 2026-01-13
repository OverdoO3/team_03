#pragma once
#include <vector>
#include "Enemy.h"
#include "SpawnPoint.h"

class WaveManager
{
public:
	WaveManager() {};
	~WaveManager() {}; 

    void Initialize();
    void Update(float elapsedTime,Stage* stage,Pathfinding* path);
    bool IsWaveCompleted() const;

	void MakeSpawnPoint(DirectX::XMFLOAT3 pos,int spawnType);

	static WaveManager& Instance()
	{
		static WaveManager instance;
		return instance;
	}

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	void DebugGUI();

private:
	std::vector<SpawnPoint::SpawnInfo> spawns = {
		{Enemy::EnemyType::Melee,3,2.0f},
		{Enemy::EnemyType::Ranged,3,2.0f},
		{Enemy::EnemyType::Melee,3,2.0f}
	};
	float startDelay;

	enum class WaveState
	{
		Waiting,    // 開始待ち
		Spawning,   // 敵生成中
		InWave,     // 全敵出現済み、残党処理
		Completed   // wave終了
	};

	std::vector<std::unique_ptr<SpawnPoint>> points;

    int currentWave = 0;

	int finishCounts = 0;

    WaveState state = WaveState::Waiting;

    float timer = 0.0f;
    int spawnIndex = 0;
    int spawnedCount = 0;
};
