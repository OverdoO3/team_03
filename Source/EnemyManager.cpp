#include "EnemyManager.h"
#include "collision.h"

void EnemyManager::Update(float elapsedTime)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	for (Enemy* enemy : removes)
	{
		std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
		if(it != enemies.end())
		{
			enemies.erase(it);
		}
		delete enemy;
	}
	removes.clear();
	CollisionEnemyVsEnemies();
}

void EnemyManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Render(rc, renderer);
	}
}

void EnemyManager::Register(Enemy* enemy)
{
	enemies.emplace_back(enemy);
}

void EnemyManager::Clear()
{
	for(Enemy * enemy : enemies)
	{
		delete enemy;
	}
}

void EnemyManager::Remove(Enemy* enemy)
{
	removes.insert(enemy);
}

void EnemyManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	for (Enemy* enemy : enemies)
	{
		enemy->RenderDebugPrimitive(rc, renderer);
	}
}

void EnemyManager::CollisionEnemyVsEnemies()
{
	EnemyManager& enemymanager = EnemyManager::Instance();

	int enemyCount = enemymanager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		for (int j = i+1; j < enemyCount; ++j)
		{
			Enemy* enemy1 = enemymanager.GetEnemy(i);
			Enemy* enemy2 = enemymanager.GetEnemy(j);

			DirectX::XMFLOAT3 outPosition;
			/*if (Collision::IntersectSphereVsSpgere(enemy1->GetPosition(), enemy1->GetRadius(), enemy2->GetPosition(), enemy2->GetRadius(), outPosition))
			{
				enemy2->SetPosition(outPosition);
			}*/
			if (Collision::IntersectCylinderVsCylinder(enemy1->GetPosition(), enemy1->GetRadius(), enemy1->GetHeight(), enemy2->GetPosition(), enemy2->GetRadius(), enemy2->GetHeight(), outPosition))
			{
				enemy2->SetPosition(outPosition);
			}
		}
	}
	/*EnemyManager& enemymanager = EnemyManager::Instance();

	int enemyCount = enemymanager.GetEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemymanager.GetEnemy(i);

		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsSpgere(position, radius, enemy->GetPosition(), enemy->GetRadius(), outPosition))
		{
			enemy->SetPosition(outPosition);
		}
	}*/
}

