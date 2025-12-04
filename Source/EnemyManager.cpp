#include "EnemyManager.h"
#include "collision.h"

void EnemyManager::Update(float elapsedTime)
{

	std::vector<Enemy*> removes;

	for (const auto& enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	for (const auto& enemy : enemies)
	{
		enemy->Update(elapsedTime);
		if (enemy->GetHP() <= 0)
		{
			removes.push_back(enemy.get());
		}
	}

	for (const auto& enemy : removes)
	{
		Remove(enemy);
	}
	removes.clear();
	CollisionEnemyVsEnemies();
}

void EnemyManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (const auto& enemy : enemies)
	{
		enemy->Render(rc, renderer);
	}
}

void EnemyManager::Register(Enemy* enemy)
{
	enemies.emplace_back(enemy);
}

void EnemyManager::Remove(Enemy* enemy)
{
	if (!enemy) return;
	bool removed = false;

	for (auto it = enemies.begin();it != enemies.end();)
	{
		if (it->get() == enemy)
		{
			it = enemies.erase(it);
			removed = true;
		}
		else
		{
			++it;
		}
	}
}

void EnemyManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	for (const auto& enemy : enemies)
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

