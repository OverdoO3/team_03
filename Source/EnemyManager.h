#pragma once

#include <vector>
#include <set>
#include "Enemy.h"
#include "nexus.h"
#include <memory>

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
	void Initialize();

	void Finalize();

	void Update(float elapsedTime,std::vector<std::unique_ptr<Tower>>& towers,std::unique_ptr<Nexus>& nexus);

	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void Register(Enemy* enemy);

	void Remove(Enemy* enemy);

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	int GetEnemyCount() const { return static_cast<int>(enemies.size());}

	Enemy* GetEnemy(int index) { return enemies.at(index).get(); }

	int GetKillCount() { return killCount; }

	const std::vector<std::unique_ptr<Enemy>>& GetEnemys() { return enemies; }

	void CollisionEnemyVsEnemies();

	Tower* FindNearestTower(const Enemy& enemy, const std::vector<std::unique_ptr<Tower>>& towers,const std::unique_ptr<Nexus>& nexus);

private:
	std::vector<std::unique_ptr<Enemy>> enemies;
	std::vector<std::unique_ptr<Enemy>> removes;

	int killCount = 0;
};