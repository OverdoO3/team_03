#include "EnemyCharge.h"
#include "DirectXCommon.h"

EnemyCharge::EnemyCharge(Stage* map, Pathfinding* pf, std::shared_ptr<Model> mod, std::shared_ptr<Effect> hiteff,std::shared_ptr<Effect> beameff)
{
	model = mod;

	hitEffect = hiteff;
	beamEffect = beameff;
	plane = std::make_unique<Plane>();

	InitializeEnemy(map, pf);

	scale.x = scale.y = scale.z = 0.02f;
	radius = 0.5f;
	height = 1.0f;

	type = EnemyType::Melee;

	health = 30;

	UpdateTransform();
	model->UpdateTransform();

	plane->SetType(3);
}

EnemyCharge::~EnemyCharge()
{
	beamEffect->Stop(beamHandle);
}

void EnemyCharge::Update(float elapsedTime)
{
	float dx = Player::Instance().GetPosition().x - position.x;
	float dz = Player::Instance().GetPosition().z - position.z;
	float dist = sqrtf(dx * dx + dz * dz);

	if (!targetTower)return;
	float tdx = targetTower->GetPosition().x - position.x;
	float tdz = targetTower->GetPosition().z - position.z;
	float tdist = sqrtf(tdx * tdx + tdz * tdz);

	plane->Update(elapsedTime, position, 10, angle.y);

	switch (state)
	{
	case EnemyCharge::State::Wander:
		UpdateEnemy(elapsedTime, targetTower);

		if (dist < rangedAttackCanRange || tdist < rangedAttackCanRange)
		{
			state = State::Charge;
			stateTimer = 1.0f;
			beamHandle = -1;
		}
		break;
	case EnemyCharge::State::Charge:
		stateTimer -= elapsedTime;
		if (beamHandle == -1)
		{
			beamHandle = beamEffect->Play(position);

			if (target)
			{
				targetPosition = Player::Instance().GetPosition();
			}
			else
			{
				targetPosition = targetTower->GetPosition();
			}

			float dx = targetPosition.x - position.x;
			float dz = targetPosition.z - position.z;
			angle.y = atan2f(dx, dz);

			DirectX::XMFLOAT3 effAngle = angle;
			effAngle.y -= DirectX::XM_PIDIV2;
	
			beamEffect->SetAngle(beamHandle, effAngle);
		}

		if (stateTimer < 0)
		{
			state = State::Attack;
			stateTimer = 2.0f;
			canDamage = true;
		}
		break;
	case EnemyCharge::State::Attack:
		UpdateAttackState(elapsedTime, *targetTower);
		stateTimer -= elapsedTime;
		if (stateTimer < 0.0f)
		{
			state = State::Wander;
		}
		break;
	}
	position.y = 0.0f;

	UpdateTransform();

	model->UpdateTransform();

	UpdateInvincibleTimer(elapsedTime);

	UpdateVelocity(elapsedTime);
}

void EnemyCharge::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model.get(), ShaderId::Lambert);

	if (state == State::Charge)
	{
		plane->Render(rc, renderer);
	}

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
		if (beamHandle == -1)
		{
			beamHandle = beamEffect->Play(position);

			float dx = Player::Instance().GetPosition().x - position.x;
			float dz = Player::Instance().GetPosition().z - position.z;
			angle.y = atan2f(dx, dz);

			DirectX::XMFLOAT3 effAngle = angle;
			effAngle.y -= DirectX::XM_PIDIV2;

			beamEffect->SetAngle(beamHandle, effAngle);
		}

		DirectX::XMFLOAT3 vec = Player::Instance().GetPosition() - position;

		vec = DirectXCommon::Normalize(vec);

		DirectX::XMFLOAT3 forword = DirectXCommon::Normalize(DirectXCommon::GetForward(transform));
		float dot = DirectXCommon::Dot(vec, forword);

		if (dot > 0.95f&&canDamage == true)
		{
			int damage = Player::Instance().GetNowRiskGauge() / 4;
			if (damage < 1) damage = 1;
			Player::Instance().ApplyDamage(damage, 0.0f);
			attackInterval = 1.0f;
			canDamage = false;
		}
	}
	else
	{
		if (beamHandle == -1)
		{
			beamHandle = beamEffect->Play(position);

			float dx = tower.GetPosition().x - position.x;
			float dz = tower.GetPosition().z - position.z;
			angle.y = atan2f(dx, dz);

			DirectX::XMFLOAT3 effAngle = angle;
			effAngle.y -= DirectX::XM_PIDIV2;

			beamEffect->SetAngle(beamHandle, effAngle);
		}

		if (canDamage == true)
		{
			targetPosition = tower.GetPosition();
			tower.TowerApplyDamage(10);
			attackInterval = 1.0f;
			canDamage = false;
		}
	}

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


