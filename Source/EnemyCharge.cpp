#include "EnemyCharge.h"

EnemyCharge::EnemyCharge(Stage* map, Pathfinding* pf)
{
	hitEffect = std::make_unique<Effect>("Data/Effect/hit.efk");
	model = std::make_unique<Model>("Data/Model/Enemy/medium_enemy.mdl");

	InitializeEnemy(map, pf);

	scale.x = scale.y = scale.z = 0.05f;
	radius = 0.5f;
	height = 1.0f;

	type = EnemyType::Melee;

	health = 100;

	UpdateTransform();

	model->UpdateTransform();
}

EnemyCharge::~EnemyCharge()
{
}

void EnemyCharge::Update(float elapsedTime, Tower& tower)
{

	float dx = Player::Instance().GetPosition().x - position.x;
	float dz = Player::Instance().GetPosition().z - position.z;
	float dist = sqrtf(dx * dx + dz * dz);

	float tdx = tower.GetPosition().x - position.x;
	float tdz = tower.GetPosition().z - position.z;
	float tdist = sqrtf(tdx * tdx + tdz * tdz);

	switch (state)
	{
	case EnemyCharge::State::Wander:
		UpdateEnemy(elapsedTime, tower);

		if (dist < rangedAttackCanRange || tdist < rangedAttackCanRange)
		{
			state = State::Attack;
			stateTimer = 1.0f;
		}
		break;
	case EnemyCharge::State::Charge:

	case EnemyCharge::State::Attack:
		UpdateAttackState(elapsedTime, tower);
		if (dist > rangedAttackCanRange && tdist > rangedAttackCanRange && stateTimer <= 0.0f)
		{
			state = State::Wander;
		}
		break;
	}

	UpdateTransform();

	model->UpdateTransform();

	UpdateInvincibleTimer(elapsedTime);

	UpdateVelocity(elapsedTime);
}

void EnemyCharge::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model.get(), ShaderId::Lambert);

	DrawDebugGUI();
}

void EnemyCharge::OnDead()
{
}

void EnemyCharge::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスのデバッグプリミティブ描画
	Enemy::RenderDebugPrimitive(rc, renderer);
	//なわばり範囲をデバッグ円柱描画
	renderer->RenderCylinder(rc, position, rangedAttackSearchRange, 1.0f, DirectX::XMFLOAT4(0, 1, 0, 1));
	renderer->RenderCylinder(rc, position, rangedAttackCanRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1));
}

void EnemyCharge::PlayHitEffect()
{
	DirectX::XMFLOAT3 pos = position;
	pos.y += 1.3f;
	hitHandle = hitEffect->Play(pos, 0.5f);
}

void EnemyCharge::SetWanderState()
{
	state = State::Wander;
}

void EnemyCharge::SetAttackState()
{
	state = State::Attack;

	stateTimer = 0.0f;
}

void EnemyCharge::UpdateAttackState(float elapsedTime, Tower& tower)
{
	float pdx = Player::Instance().GetPosition().x - position.x;
	float pdz = Player::Instance().GetPosition().z - position.z;
	float pdist = sqrtf(pdx * pdx + pdz * pdz);

	float tdx = tower.GetPosition().x - position.x;
	float tdz = tower.GetPosition().z - position.z;
	float tdist = sqrtf(tdx * tdx + tdz * tdz);

	if (pdist < tdist)
	{
		targetPosition = Player::Instance().GetPosition();
		if (attackInterval < 0.0f)
		{
			Player::Instance().ApplyDamage(1, 0.0f);
			attackInterval = 1.0f;
		}
	}
	else
	{
		targetPosition = tower.GetPosition();
		if (attackInterval < 0.0f)
		{
			tower.TowerApplyDamage(10);
			attackInterval = 1.0f;
		}
	}

	attackInterval -= elapsedTime;

	MoveToTarget(elapsedTime, 0.0f, 1.0f);

	stateTimer -= elapsedTime;
}

void EnemyCharge::DrawDebugGUI()
{
}

void EnemyCharge::MoveToTarget(float elapsedTime, float moveSpeedRate, float turnSpeedRate)
{
	//ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	//移動処理
	Move(elapsedTime, vx, vz, moveSpeed * moveSpeedRate);
	Turn(elapsedTime, vx, vz, turnSpeed * turnSpeedRate);
}


