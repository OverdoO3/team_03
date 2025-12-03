#pragma once

#include "System/Model.h"
#include "Enemy.h"
#include "Player.h"	
#include "ProjectileManager.h"

class EnemySlime : public Enemy
{
public:
	EnemySlime();
	~EnemySlime() override;

	void Update(float elapsedTime) override;

	void Render(const RenderContext& rc, ModelRenderer* renderer)override;

	void OnDead() override;

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer) override;

	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);
private:
	void SetRandamTargetPosition();
	void MoveToTarget(float elapsedTime, float moveSpeedRate, float turnSpeedRate);
	void SetWanderState();
	void UpdateWanderState(float elapsedTime);
	void SetIdleState();
	void UpdateIdleState(float elapsedTime);
	bool SearchPlayer();
	void SetAttackState();
	void UpdateAttackState(float elapsedTime);

	enum class State
	{
		Wander,
		Idle,
		Attack
	};


	State state = State::Wander;
	DirectX::XMFLOAT3 targetPosition = { 0,0,0 };
	DirectX::XMFLOAT3 territoryOrigine = { 0,0,0 };
	float territoryRange = 10.0f;
	float moveSpeed = 2.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 5.0f;
	ProjectileManager projectileManager;
};