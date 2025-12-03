#pragma once

#include <vector>
#include <set>
#include "Enemy.h"

class EnemyManager
{
private:
	EnemyManager() {}
	~EnemyManager() {}
public:
	//インスタンス
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}

	void Update(float elapsedTime);

	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void Register(Enemy* enemy);

	void Clear();

	void Remove(Enemy* enemy);

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	int GetEnemyCount() const { return static_cast<int>(enemies.size());}

	Enemy* GetEnemy(int index) { return enemies.at(index); }

	void CollisionEnemyVsEnemies();
private:
	std::vector<Enemy*> enemies;

	std::set<Enemy*> removes;
};