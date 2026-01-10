#include "EnemySlime.h"
#include "MathUtils.h"
#include "Player.h"
#include "ProjectileStraight.h"
#include <imgui.h>

EnemySlime::EnemySlime(Stage* map, Pathfinding* pf)
{
	hitEffect = std::make_unique<Effect>("Data/Effect/hit.efk");
	model = std::make_unique<Model>("Data/Model/Slime/Slime.mdl");
	
	InitializeEnemy(map, pf);

	scale.x = scale.y = scale.z = 0.01f;
	radius = 0.5f;
	height = 1.0f;

	health = 100;
}

EnemySlime::~EnemySlime()
{
}

void EnemySlime::Update(float elapsedTime, Tower& tower)
{
	float dx = Player::Instance().GetPosition().x - position.x;
	float dz = Player::Instance().GetPosition().z - position.z;
	float dist = sqrtf(dx * dx + dz * dz);

	float tdx = tower.GetPosition().x - position.x;
	float tdz = tower.GetPosition().z - position.z;
	float tdist = sqrtf(tdx * tdx + tdz * tdz);

	switch (state)
	{
	case EnemySlime::State::Wander:
		UpdateEnemy(elapsedTime, tower);
		
		if (dist < attackCanRange || tdist < attackCanRange)
		{
			state = State::Attack;
			stateTimer = 1.0f;
		}
		break;
	case EnemySlime::State::Attack:
		UpdateAttackState(elapsedTime,tower);
		if (dist > attackCanRange&&tdist > attackCanRange&& stateTimer <= 0.0f)
		{
			state = State::Wander;
		}
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

	DrawDebugGUI();
}

void EnemySlime::OnDead()
{
	
}

void EnemySlime::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスのデバッグプリミティブ描画
	Enemy::RenderDebugPrimitive(rc, renderer);
	//なわばり範囲をデバッグ円柱描画
	renderer->RenderCylinder(rc, position, attackSearchRange, 1.0f, DirectX::XMFLOAT4(0, 1, 0, 1));
	renderer->RenderCylinder(rc, position, attackCanRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1));
	//ターゲット位置をデバッグ球描画
	if (targetPosition.x != FLT_MAX)
	{
		renderer->RenderSphere(rc, targetPosition, 1.0f, DirectX::XMFLOAT4(1, 1, 0, 1));
	}
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
}

void EnemySlime::SetAttackState()
{
	state = State::Attack;

	stateTimer = 0.0f;
}

void EnemySlime::UpdateAttackState(float elapsedTime,Tower& tower)
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

void EnemySlime::PlayHitEffect()
{
	DirectX::XMFLOAT3 pos = position;
	pos.y += 1.3f;
	hitHandle = hitEffect->Play(pos, 0.5f);
}

void EnemySlime::DrawDebugGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		ImGui::InputInt("EnemyNow", &health);
		ImGui::End();
	}
}


