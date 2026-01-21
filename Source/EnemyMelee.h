#pragma once

#include "System/Model.h"
#include "Enemy.h"
#include "Player.h"	
#include "ProjectileManager.h"

class EnemyMelee : public Enemy
{
public:
	std::shared_ptr<Effect> hitEffect;
	Effekseer::Handle hitHandle = -1;
public:
	EnemyMelee(Stage* map, Pathfinding* pf,std::shared_ptr<Model> mod,std::shared_ptr<Effect> hiteff);
	~EnemyMelee() override;

	void Update(float elapsedTime) override;

	void Render(const RenderContext& rc, ModelRenderer* renderer)override;

	void OnDead() override;

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer) override;

	void PlayHitEffect();
private:
	void SetWanderState();
	void SetAttackState();
	void UpdateAttackState(float elapsedTime,Tower& tower);
	void DrawDebugGUI();

	void MoveToTarget(float elapsedTime, float moveSpeedRate, float turnSpeedRate);

	enum class State
	{
		Wander,
		Attack
	};
	State state = State::Wander;

	std::shared_ptr<Model> nowModel = nullptr;

	DirectX::XMFLOAT3 targetPosition = { 0,0,0 };
	float moveSpeed = 2.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;

	float attackInterval = 0.0f;
	 
	ProjectileManager projectileManager;
};