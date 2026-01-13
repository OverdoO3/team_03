#include "SpawnPoint.h"

SpawnPoint::SpawnPoint(SpawnInfo in)
{
	//position = { 6,0,0 };
	info = in;
	UpdateTransfomEuler();
}

SpawnPoint::~SpawnPoint()
{
}

void SpawnPoint::Update(float elapsedTime, Stage* stage, Pathfinding* path)
{
	timer += elapsedTime;
	if (timer > info.interval && count < info.count)
	{
		timer = 0.0f;
		SpawnEnemy(stage,path);
	}
}

void SpawnPoint::SpawnEnemy(Stage* stage, Pathfinding* path)
{
	Enemy* enemy;
	switch (info.type)
	{
	case Enemy::EnemyType::Melee:
		enemy = new EnemyMelee(stage, path);
		enemy->InitializeEnemy(stage, path);
		enemy->SetPosition(position);
		enemy->SetReady(true);
		EnemyManager::Instance().Register(enemy);
		break;
	case Enemy::EnemyType::Ranged:

		enemy = new EnemyCharge(stage, path);
		enemy->InitializeEnemy(stage, path);
		enemy->SetPosition(position);
		enemy->SetReady(true);
		EnemyManager::Instance().Register(enemy);
		break;
	}
	
	count++;
}

bool SpawnPoint::isFinish()
{
	if (count == info.count)
	{
		count = 0;
		return true;
	}
	return false;
}

void SpawnPoint::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	renderer->RenderBox(rc, position, angle, { 1,1,1 }, { 1,1,0,1 });
}
