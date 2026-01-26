#pragma once
#include "Obj.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "EnemyMelee.h"
#include "EnemyCharge.h"
#include "Pathfinding.h"
#include "Stage.h"

class SpawnPoint : public Obj
{
public:
	struct SpawnInfo
	{
		Enemy::EnemyType type;
		int count;
		float interval;
	};

	SpawnPoint(SpawnInfo in);
	~SpawnPoint() override;

	void Update(float elapsedTime,Stage* stage, Pathfinding* path,int waveCount);

	void SpawnEnemy(Stage* stage, Pathfinding* path);

	bool isFinish(int waveCount);

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer) override;

	int getCount() { return count; }
private:
	SpawnInfo info;
	
	float timer = 0.0f;
	int count = 0;

	std::shared_ptr<Model>enemyMeleeModel = nullptr;
	std::shared_ptr<Model>enemyRangeModel = nullptr;

	std::shared_ptr<Effect> hitEffect = nullptr;
	std::shared_ptr<Effect> beamEffect = nullptr;
};