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

	void Update(float elapsedTime,Stage* stage, Pathfinding* path);

	void SpawnEnemy(Stage* stage, Pathfinding* path);

	bool isFinish();

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer) override;

	int getCount() { return count; }
private:
	SpawnInfo info;
	
	float timer = 0.0f;
	int count = 0;
};