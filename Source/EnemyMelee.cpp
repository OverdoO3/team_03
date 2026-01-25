#include "EnemyMelee.h"
#include "MathUtils.h"
#include "Player.h"
#include "ProjectileStraight.h"
#include <imgui.h>

EnemyMelee::EnemyMelee(Stage* map, Pathfinding* pf, std::shared_ptr<Model> mod, std::shared_ptr<Effect> hiteff)
{
	hitEffect = hiteff;
	model = mod;

	InitializeEnemy(map, pf);

	scale.x = scale.y = scale.z = 0.01f;
	radius = 0.5f;
	height = 1.0f;

	type = EnemyType::Melee;

	health = 100;

	PlayAnimation("walk", true);

	UpdateTransform();
	model->UpdateTransform();
}

EnemyMelee::~EnemyMelee()
{
}

void EnemyMelee::Update(float elapsedTime)
{
	float dx = Player::Instance().GetPosition().x - position.x;
	float dz = Player::Instance().GetPosition().z - position.z;
	float dist = sqrtf(dx * dx + dz * dz);

	if (!targetTower)return;
	float tdx = targetTower->GetPosition().x - position.x;
	float tdz = targetTower->GetPosition().z - position.z;
	float tdist = sqrtf(tdx * tdx + tdz * tdz);

	switch (state)
	{
	case EnemyMelee::State::Wander:
		UpdateEnemy(elapsedTime, targetTower);
		if (dist < meleeAttackCanRange || tdist < meleeAttackCanRange)
		{
			state = State::Attack;
			PlayAnimation("attack", false);
			stateTimer = 1.0f;
		}
		break;
	case EnemyMelee::State::Attack:
		UpdateAttackState(elapsedTime,*targetTower);
		if (dist > meleeAttackCanRange&&tdist > meleeAttackCanRange&& stateTimer <= 0.0f)
		{
			enemyWepon.SetIsAttack(false);
			state = State::Wander;
			PlayAnimation("walk", true);
		}
		break;
	}

	UpdateTransform();

	model->UpdateTransform();

	UpdateInvincibleTimer(elapsedTime);

	UpdateVelocity(elapsedTime);

	UpdateAnimation(elapsedTime);
}

void EnemyMelee::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model.get(), ShaderId::Lambert);

	DrawDebugGUI();
}

void EnemyMelee::OnDead()
{
	
}

void EnemyMelee::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスのデバッグプリミティブ描画
	Enemy::RenderDebugPrimitive(rc, renderer);
	//なわばり範囲をデバッグ円柱描画
	renderer->RenderCylinder(rc, position, meleeAttackSearchRange, 1.0f, DirectX::XMFLOAT4(0, 1, 0, 1));
	renderer->RenderCylinder(rc, position, meleeAttackCanRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1));

	enemyWepon.RenderDebugPrimitive(rc, renderer);
}

void EnemyMelee::MoveToTarget(float elapsedTime, float moveSpeedRate, float turnSpeedRate)
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

void EnemyMelee::SetWanderState()
{
	state = State::Wander;
}

void EnemyMelee::SetAttackState()
{
	state = State::Attack;

	stateTimer = 0.0f;
}

void EnemyMelee::UpdateAttackState(float elapsedTime,Tower& tower)
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
			enemyWepon.SetIsAttack(true);
			enemyWepon.SetPosition(position);
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

void EnemyMelee::PlayHitEffect()
{
	DirectX::XMFLOAT3 pos = position;
	pos.y += 1.3f;
	hitHandle = hitEffect->Play(pos, 0.5f);
}

void EnemyMelee::DrawDebugGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		ImGui::InputInt("EnemyHP", &health);
		ImGui::End();
	}
}


