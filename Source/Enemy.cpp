#include "Enemy.h"
#include "EnemyManager.h"
#include "Player.h"

void Enemy::InitializeEnemy(Stage* map, Pathfinding* pf)
{
	stage = map;
	pathfinding = pf;

	assert(pathfinding);
}

void Enemy::UpdateEnemy(float dt,Tower& tower)
{
	if (!ready)return;
	pathTimer += dt;

	float dx1 = Player::Instance().GetPosition().x - position.x;
	float dz1 = Player::Instance().GetPosition().z - position.z;
	float dist1 = sqrtf(dx1 * dx1 + dz1 * dz1);

	XMFLOAT3 targetPos{};
	if (attackSearchRange > dist1)
	{
		targetPos = Player::Instance().GetPosition();
		target = true;
	}
	else
	{
		DirectX::XMFLOAT3 tar = tower.GetPosition();
		tar.x += 1.0f;
		tar.z += 1.0f;

		targetPos = tar;
		target = false;
	}

	if (pathTimer >= pathInterval)
	{
		RequestPath(targetPos);
		pathTimer = 0.0f;
	}

	repathCooldown -= dt;

	// パスが無いなら取得
	if (path.empty())
	{
		RequestPath(targetPos);
		return;
	}

	MoveAlongPath(dt);

	// ==== スタック判定 ====

	float dx = position.x - lastPosition.x;
	float dz = position.z - lastPosition.z;
	float moved = sqrtf(dx * dx + dz * dz);

	if (moved < 0.01f)
	{
		stuckTimer += dt;
	}
	else
	{
		stuckTimer = 0.0f;
		lastPosition = position;
	}

	// ==== 一定時間進めなかったら再探索 ====
	if (stuckTimer > 0.5f && repathCooldown <= 0.0f)
	{
		RequestPath(targetPos);
		repathCooldown = 0.5f;
		stuckTimer = 0.0f;
	}

	MoveAlongPath(dt);
}

void Enemy::RequestPath(const XMFLOAT3& targetPos)
{
	int sx, sz, gx, gz;

	if (!stage->WorldToGrid(position, sx, sz)) return;
	if (!stage->WorldToGrid(targetPos, gx, gz)) return;

	path = pathfinding->FindPath(sx, sz, gx, gz);
	debugPath = pathfinding->FindPath(sx, sz, gx, gz);
	pathIndex = 1;
}

void Enemy::MoveAlongPath(float elapsedTime)
{
	if (path.empty() || pathIndex >= path.size()) return;

	Node* node = path[pathIndex];
	XMFLOAT3 target = stage->GridToWorld(node->x, node->z);

	if (MoveTowards(target, moveSpeed, elapsedTime))
	{
		pathIndex++;
	}
}


void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}
