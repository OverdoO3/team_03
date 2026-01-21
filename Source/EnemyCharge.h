#pragma once
#include "System/Model.h"
#include "Enemy.h"
#include "Player.h"	
#include "ProjectileManager.h"
#include "Plane.h"

class EnemyCharge : public Enemy
{
public:
	EnemyCharge(Stage* map, Pathfinding* pf, std::shared_ptr<Model> mod, std::shared_ptr<Effect> hiteff,std::shared_ptr<Effect> beameff);
	~EnemyCharge() override;

	void Update(float elapsedTime) override;

	void Render(const RenderContext& rc, ModelRenderer* renderer)override;

	void OnDead() override;

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer) override;

	void PlayHitEffect();
private:
	void SetWanderState();
	void SetAttackState();
	void UpdateAttackState(float elapsedTime, Tower& tower);
	void DrawDebugGUI();

	void MoveToTarget(float elapsedTime, float moveSpeedRate, float turnSpeedRate);

private:
	enum class State
	{
		Wander,
		Charge,
		Attack,
	};
	State state = State::Wander;

	DirectX::XMFLOAT3 targetPosition = { 0,0,0 };
	float moveSpeed = 20.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;

	float attackInterval = 0.0f;
	float beamRange = 10.0f;

	std::unique_ptr<Plane> plane;

	std::shared_ptr<Effect> hitEffect;
	Effekseer::Handle hitHandle = -1;

	std::shared_ptr<Effect> beamEffect;
	Effekseer::Handle beamHandle = -1;

	bool canDamage;
};