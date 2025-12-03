#include "EnemySlime.h"
#include "MathUtils.h"
#include "Player.h"
#include "ProjectileStraight.h"

EnemySlime::EnemySlime()
{
	model = std::make_unique<Model>("Data/Model/Slime/Slime.mdl");
	
	scale.x = scale.y = scale.z = 0.01f;
	radius = 0.5f;
	height = 1.0f;

	//徘徊ステートへ遷移
	SetWanderState();
}

EnemySlime::~EnemySlime()
{
}

void EnemySlime::Update(float elapsedTime)
{
	switch (state)
	{
	case State::Wander:
		UpdateWanderState(elapsedTime);
		break;
	case State::Idle:
		UpdateIdleState(elapsedTime);
		break;
	case State::Attack:
		UpdateAttackState(elapsedTime);
		break;
	}
	UpdateTransform();

	model->UpdateTransform();

	UpdateInvincibleTimer(elapsedTime);

	UpdateVelocity(elapsedTime);

	projectileManager.Update(elapsedTime);
}

void EnemySlime::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model.get(), ShaderId::Lambert);

	//弾丸描画
	projectileManager.Render(rc, renderer);
}

void EnemySlime::OnDead()
{
	//自身を破棄
	Destroy();
}

void EnemySlime::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスのデバッグプリミティブ描画
	Enemy::RenderDebugPrimitive(rc, renderer);
	//なわばり範囲をデバッグ円柱描画
	renderer->RenderCylinder(rc, territoryOrigine, territoryRange, 1.0f,DirectX::XMFLOAT4(0,1,0,1));
	//ターゲット位置をデバッグ球描画
	if (targetPosition.x != FLT_MAX)
	{
		renderer->RenderSphere(rc, targetPosition, 1.0f, DirectX::XMFLOAT4(1, 1, 0, 1));
	}
	//索敵範囲をデバッグ円描画
	renderer->RenderCylinder(rc, position, searchRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1));
}

void EnemySlime::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigine = origin;
	territoryRange = range;
}

void EnemySlime::SetRandamTargetPosition()
{
	float theta = MathUtils::RandomRangeFloat(-DirectX::XM_PI, DirectX::XM_PI);
	float range = MathUtils::RandomRangeFloat(0.0f, territoryRange);
	targetPosition.x = territoryOrigine.x + sinf(theta) * range;
	targetPosition.y = territoryOrigine.y;
	targetPosition.z = territoryOrigine.z + cosf(theta) * range;
}

void EnemySlime::MoveToTarget(float elapsedTime, float moveSpeedRate, float turnSpeedRate)
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

void EnemySlime::SetWanderState()
{
	state = State::Wander;

	//目標地点
	SetRandamTargetPosition();
}

void EnemySlime::UpdateWanderState(float elapsedTime)
{
	//目標地点までXZ平面での距離判定
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;
	if (distSq < radius * radius)
	{
		//次の目標地点設定
		SetIdleState();
	}
	if (SearchPlayer())
	{
		SetAttackState();
	}
	MoveToTarget(elapsedTime,1.0f,1.0f);
}

void EnemySlime::SetIdleState()
{
	state = State::Idle;

	//タイマー
	stateTimer = MathUtils::RandomRangeFloat(3.0f, 5.0f);
}

//待機更新
void EnemySlime::UpdateIdleState(float elapsedTime)
{
	stateTimer -= elapsedTime;
	if (stateTimer < 0.0f)
	{
		//徘徊へ遷移
		SetWanderState();
	}
	if (SearchPlayer())
	{
		targetPosition = { FLT_MAX, FLT_MAX, FLT_MAX };
		SetAttackState();
	}
}
bool EnemySlime::SearchPlayer()
{
	//プレイヤーとの皇帝さを考慮して3Dでの距離判定をする
	const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();
	float vx = playerPosition.x - position.x;
	float vy = playerPosition.y - position.y;
	float vz = playerPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);
	if (dist < searchRange)
	{
		float distXZ = sqrtf(vx * vx + vz * vz);
		//単位ベクトル化
		vx /= distXZ;
		vz /= distXZ;
		//前方ベクトル
		float frontX = sinf(angle.y);
		float frontZ = cosf(angle.y);
		//2つのベクトルの内積値で前後判定
		float dot = (frontX * vx) + (frontZ * vz);
		if (dot > 0.0f)
		{
			return true;
		}
	}
	return false;
}

void EnemySlime::SetAttackState()
{
	state = State::Attack;

	stateTimer = 0.0f;
}

void EnemySlime::UpdateAttackState(float elapsedTime)
{
	targetPosition = Player::Instance().GetPosition();

	MoveToTarget(elapsedTime, 0.0f, 1.0f);

	stateTimer -= elapsedTime;
	if (stateTimer < 0.0f)
	{
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);
		//発射方向
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;
		//発射
		ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
		projectile->Launch(dir, pos);

		stateTimer = 2.0f;
	}
	if (!SearchPlayer())
	{
		targetPosition = { FLT_MAX, FLT_MAX, FLT_MAX };
		SetIdleState();
	}
}


