#include "SpawnPoint.h"

SpawnPoint::SpawnPoint(SpawnInfo in)
{
	//position = { 6,0,0 };
	info = in;
	UpdateTransfomEuler();

	enemyMeleeModel = std::make_shared<Model>("Data/Model/Enemy/small_enemy.mdl");
	enemyRangeModel = std::make_shared<Model>("Data/Model/Enemy/medium_enemy.mdl");

	hitEffect = std::make_unique<Effect>("Data/Effect/hit.efk");
	beamEffect = std::make_unique<Effect>("Data/Effect/enemy_beam/enemy_beam.efk");
}

SpawnPoint::~SpawnPoint()
{
}

void SpawnPoint::Update(float elapsedTime, Stage* stage, Pathfinding* path,int waveCount)
{
	timer += elapsedTime;
	if (timer > info.interval && count < info.count * waveCount)
	{
		timer = 0.0f;
		SpawnEnemy(stage,path);
	}
}

void SpawnPoint::SpawnEnemy(Stage* stage, Pathfinding* path)
{
	Enemy* enemy;
	int a = rand() % 2;
	switch (a)
	{
	case 0:
		enemy = new EnemyMelee(stage, path,enemyMeleeModel,hitEffect);
		enemy->SetPosition(position);
		enemy->SetReady(true);
		EnemyManager::Instance().Register(enemy);
		break;
	case 1:
		enemy = new EnemyCharge(stage, path,enemyRangeModel,hitEffect,beamEffect);
		enemy->SetPosition(position);
		enemy->SetReady(true);
		EnemyManager::Instance().Register(enemy);
		break;
	}
	
	count++;
}

bool SpawnPoint::isFinish(int waveCount)
{
	if (count == info.count * waveCount)
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
