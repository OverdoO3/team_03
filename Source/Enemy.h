#pragma once

#include "System/ModelRenderer.h"
#include "Character.h"
#include <vector>
#include "DirectXMath.h"
#include "Stage.h"
#include "Pathfinding.h"
#include "tower.h"

using namespace DirectX;

//エネミー
class Enemy : public Character
{
public:

	Enemy() {}
	~Enemy() override {}

	virtual void Update(float elapsedTime,Tower& tower) = 0;

	void InitializeEnemy(Stage* map, Pathfinding* pf);
	virtual void UpdateEnemy(float dt,Tower& tower);

	//破棄
	void Destroy();

	virtual void Render(const RenderContext& rc, ModelRenderer* renderer) = 0;

	virtual void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
	{
		// 共通のデバッグ描画（例：当たり判定の円柱）
		renderer->RenderCylinder(rc, GetPosition(), GetRadius(), GetHeight(), DirectX::XMFLOAT4(0, 0, 0, 1));
		const float boxSize = 0.3f;

		for (Node* n : debugPath)
		{
			DirectX::XMFLOAT3 pos =
				stage->GridToWorld(n->x, n->z);

			pos.y += 0.05f; // 床から少し浮かせる

			renderer->RenderBox(
				rc,
				pos,
				{ 0,0,0 },
				{ boxSize, boxSize, boxSize },
				{ 1, 0, 0, 0.8f } // 赤
			);
		}
	}

	virtual void PlayHitEffect() {}

	bool wasHit = false;

	void SetReady(bool a) { ready = a; }

	bool target = true;
protected:
	void RequestPath(const DirectX::XMFLOAT3& playerPos);
	void MoveAlongPath(float elapsedTime);

protected:
	Stage* stage = nullptr;
	Pathfinding* pathfinding = nullptr;

	std::vector<Node*> path;
	int pathIndex = 0;

	DirectX::XMFLOAT3 lastPosition{};
	float stuckTimer = 0.0f;

	float repathCooldown = 0.0f;

	float pathTimer = 0.0f;
	float pathInterval = 0.5f;

	float moveSpeed = 2.0f;

	bool ready = false;

	float attackSearchRange = 7.0f;
	float attackCanRange = 3.0f;

	std::vector<Node*> debugPath;
};