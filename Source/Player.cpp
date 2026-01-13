#include <imgui.h>

#include "player.h"
#include "Camera.h"
#include "System/Input.h"
#include "EnemyManager.h"
#include "collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"
#include "System/Audio.h"
#include "CursorManager.h"
#include "RayCast.h"
#include "Screen.h"
#include "KeyInput.h"

void Player::Initialize()
{
	wepons[0] = std::make_unique<Model>("Data/Model/Player/sword_character.mdl");
	wepons[1] = std::make_unique<Model>("Data/Model/Player/axe_character.mdl");
	wepons[2] = std::make_unique<Model>("Data/Model/Player/spear_character.mdl");

	model = wepons[(int)nowWepon];
	trailEffect= std::make_unique<Effect>("Data/Effect/trail_demo.efk");
	WeponTrailEffect= std::make_unique<Effect>("Data/Effect/wepon.efk");
	hitSE.reset(Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav"));

	scale.x = scale.y = scale.z = 0.05f;
	trailEffect->SetPosition(trailHandle, position);
	trailHandle = trailEffect->Play({ 0,5.0f,0 }, 0.5f);

	col = std::make_unique<Wepon>();

	position = { 5,3,3 };

	state = State::Idle;
	PlayAnimation("Idle", true);
}

void Player::Finalize()
{
}

void Player::Update(float elapsedTime, const int(&maps)[38][38])
{
	UpdateVelocity(elapsedTime);

	CollisionPlyerVsEnemies();

	//InputMove(elapsedTime);

	CollisionWeponVsEnemies();

	trailEffect->SetPosition(trailHandle, position);

	col->Update(elapsedTime);

	ChangeWepon();

	switch (state)
	{
	case State::Idle:
	{
		if (!isChargeRush)
		{
			if (InputMove(elapsedTime, maps))
			{
				state = State::Run;
				PlayAnimation("Running", true);
			}
		}
		switch (nowWepon)
		{
		case HaveWepon::Axe:
			InputCharge(elapsedTime);
			break;
		case HaveWepon::Spere:
			InputRush(elapsedTime, maps);
			break;
		case HaveWepon::Sword:
			break;
		default:
			break;
		}
		InputAvoid();
		InputAttack();
		break;
	}
	case State::Run:
	{
		if (!isChargeRush)
		{
			if (InputMove(elapsedTime, maps))
			{
				state = State::Idle;
				PlayAnimation("Idle", true);
			}
		}

		switch (nowWepon)
		{
		case HaveWepon::Axe:
			InputCharge(elapsedTime);
			break;
		case HaveWepon::Spere:
			InputRush(elapsedTime, maps);
			break;
		case HaveWepon::Sword:
			break;
		default:
			break;
		}
		InputAvoid();
		InputAttack();
		moveSpeed = 5.0f;
		break;
	}

	case State::Avoid:
	{
		avoidTimer -= elapsedTime;
		MoveWithCollision(elapsedTime, avoidVec.x, avoidVec.z, maps, 5.0f);
		col->SetIsAttack(false);
		if (avoidTimer <= 0.0f)
		{
			velocity.x = 0;
			velocity.z = 0;
			chargeValue = 1.0f;
			chargeTime = 0.0f;
			chargestil = 1.0f;
			
			state = State::Idle;
		}
		break;
	}

	case State::Attack:
	{
		if (!animationPlaying)
		{
			state = State::Idle;
			PlayAnimation("Idle", true);
			break;
		}
		InputAttack();
		InputRush(elapsedTime, maps);
		if (InputMove(elapsedTime, maps))
		{
			state = State::Run;
			PlayAnimation("Running", true);
		}
		moveSpeed = 2.0f;
		break;
	}

	case State::Rush:
	{
		//突進
		float dx = rushVec.x * rushSpeed * rushDist * elapsedTime;
		float dz = rushVec.z * rushSpeed * rushDist * elapsedTime;

		MoveWithCollision(elapsedTime, dx, dz, maps, 10.0f);

		rushTimer -= elapsedTime;
		col->SetPosition(position);
		if (rushTimer <= 0.0f)
		{
			state = State::Idle;
			PlayAnimation("Idle", true);
			rushDist = 0.0f;
		}
		break;
	}
	case State::Charge:
	{
		InputCharge(elapsedTime);
		InputAvoid();
		chargestil -= elapsedTime;
		if (chargestil <= 0.0f)
		{
			chargeValue = 1.0f;
			chargeTime = 0.0f;
			chargestil = 1.0f;
			col->SetRadius(1.0f);
			state = State::Idle;
		}
	}
}

	// トランスフォーム更
	model->UpdateTransform();

	UpdateTransform();

	UpdateAnimation(elapsedTime);

	invincibleTime -= elapsedTime;
}

void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model.get(), ShaderId::Lambert);

	for (auto& n : model->GetNodes())
	{
		if (strcmp(n.name, "kensaki") == 0&&col->GetIsAttack())
		{
			// モデル内部の transform
			DirectX::XMMATRIX local = DirectX::XMLoadFloat4x4(&n.globalTransform);

			// プレイヤーの world transform を XMMATRIX に変換
			DirectX::XMMATRIX playerWorld = DirectX::XMLoadFloat4x4(&transform);

			// ワールド座標 = プレイヤーのワールド行列 * ノードのグローバル行列
			DirectX::XMMATRIX worldMat = local * playerWorld;

			// 座標を抽出
			WeponTipPos.x = worldMat.r[3].m128_f32[0];
			WeponTipPos.y = worldMat.r[3].m128_f32[1];
			WeponTipPos.z = worldMat.r[3].m128_f32[2];

			// 軌跡発生
			WeponTrailEffect->Play(WeponTipPos, 1.0f);
		}
	}
}

void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスの関数呼び出し
	Character::RenderDebugPrimitive(rc, renderer);

	col->RenderDebugPrimitive(rc, renderer);
}

void Player::InputAttack()
{
	//if (GetAsyncKeyState(VK_LBUTTON))
	if (KeyInput::Instance().GetKeyDown(VK_LBUTTON))
	{
		Enemy* nearestEnemy = nullptr;
		float nearestDist = FLT_MAX;
		for (const auto& enemy : EnemyManager::Instance().GetEnemys())
		{
			DirectX::XMFLOAT3 p = enemy->GetPosition();
			float dx = p.x - position.x;
			float dy = p.y - position.y;
			float dz = p.z - position.z;
			float currentDist = sqrtf(dx * dx + dy * dy + dz * dz);

			// もっと近いものが見つかったら更新
			if (currentDist < nearestDist) {
				nearestDist = currentDist;
				nearestEnemy = enemy.get();
			}
		}
		if (nearestEnemy != nullptr)
		{
			DirectX::XMFLOAT3 n = nearestEnemy->GetPosition();
			DirectX::XMFLOAT3 vec = { position.x - n.x,position.y - n.y,position.z - n.z };

			// XZ 平面に投影
			float dx = vec.x;
			float dz = vec.z;

			// Y軸まわりの回転角
			angle.y = atan2f(-dx, -dz); // ※向きによって順番調整
		}

		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y) * 2;
		dir.y = 0.0f;
		dir.z = cosf(angle.y) * 2;

		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + 0.8f;
		pos.z = position.z;

		col->SetPosition({ pos.x + dir.x ,pos.y + dir.y ,pos.z + dir.z });
		col->SetIsAttack(true);
		col->SetTimer(0.2f);

		state = State::Attack;
		PlayAnimation("Attack", false);

		DirectX::XMFLOAT3 p = position;
		p.y = position.y + 1.0f;
		//WeponTrailEffect->Play(p, 1.0f);
		DirectX::XMFLOAT3 a = angle;
		a.x = DirectX::XM_PI / 2;
		col->SetAngle(a);
	}
}

void Player::InputAvoid()
{
	if (avoidTimer <= 0.0f&&KeyInput::Instance().GetKeyDown(VK_LSHIFT))
	{
		avoidTimer = 0.2f;
		state = State::Avoid;
		avoidVec = GetMoveVec();
		//PlayAnimation("Avoid", false);
	}
}

void Player::InputRush(float elapsedTime, const int(&maps)[38][38])
{
	//右クリック押している間向きをカーソルの方向にするやーつ

	DirectX::XMFLOAT3 vec{};
	if (KeyInput::Instance().GetKeyHold(VK_RBUTTON))
	{
		DirectX::XMFLOAT3 curPosS = Screen::GetScreenCursorWorld(&Camera::Instance(), 0);
		DirectX::XMFLOAT3 curPosE = Screen::GetScreenCursorWorld(&Camera::Instance(), 1.0f);

		DirectX::XMFLOAT3 hitPosition;
		DirectX::XMFLOAT3 hitNormal;

		if (Hit::RayCast(curPosS, curPosE,
			stage->GetTransform(),
			stage->getModel(),
			hitPosition, hitNormal))
		{
			vec = {
				hitPosition.x - position.x,
				0.0f,
				hitPosition.z - position.z
			};

			// 正規化
			float len = sqrtf(vec.x * vec.x + vec.z * vec.z);
			if (len != 0.0f)
			{
				vec.x /= len;
				vec.z /= len;
			}

			rushVec = vec;

			// 回転
			float angles = atan2f(vec.x, vec.z);
			angle.y = angles;
		}
		moveSpeed = 2.0f;
		rushDist += elapsedTime;

		isChargeRush = true;
	}

	// 離した瞬間：突進
	if (KeyInput::Instance().GetKeyUp(VK_RBUTTON))
	{
		state = State::Rush;
		rushTimer = rushTime * rushDist;
		moveSpeed = 5.0f;
		isChargeRush = false;
		col->SetIsAttack(true);
		col->SetTimer(rushTimer);
		//PlayAnimation("Attack", false);
	}
}

void Player::InputCharge(float elapsedTime)
{
	//右クリック押している間向きをカーソルの方向にするやーつ

	DirectX::XMFLOAT3 vec{};
	if (KeyInput::Instance().GetKeyHold(VK_RBUTTON))
	{
		state = State::Charge;
		DirectX::XMFLOAT3 curPosS = Screen::GetScreenCursorWorld(&Camera::Instance(), 0);
		DirectX::XMFLOAT3 curPosE = Screen::GetScreenCursorWorld(&Camera::Instance(), 1.0f);

		DirectX::XMFLOAT3 hitPosition;
		DirectX::XMFLOAT3 hitNormal;

		if (Hit::RayCast(curPosS, curPosE,
			stage->GetTransform(),
			stage->getModel(),
			hitPosition, hitNormal))
		{
			vec = {
				hitPosition.x - position.x,
				0.0f,
				hitPosition.z - position.z
			};

			// 正規化
			float len = sqrtf(vec.x * vec.x + vec.z * vec.z);
			if (len != 0.0f)
			{
				vec.x /= len;
				vec.z /= len;
			}

			rushVec = vec;

			// 回転
			float angles = atan2f(vec.x, vec.z);
		
			angle.y = angles;

			chargeTime += elapsedTime;
		}
		chargestil = 0.5f;
		col->SetIsCharge(true);
		col->SetRadius(chargeTime * 1.0f);
		col->SetAngle({ 0,0,0 });
	}

	// 離した瞬間開放
	if (KeyInput::Instance().GetKeyUp(VK_RBUTTON))
	{
		if (chargeTime > 2.5f)
		{
			chargeValue = 9.0f;
		}
		else if (chargeTime > 1.8f)
		{
			chargeValue = 6.0f;
		}
		else if (chargeTime > 1.0f)
		{
			chargeValue = 3.5f;
		}
		else if (chargeTime > 0.5f)
		{
			chargeValue = 2.0f;
		}
		else
		{
			chargeValue = 1.0f;
		}

		chargestil = 0.5f;
		col->SetIsCharge(false);
		col->SetIsAttack(true);
		col->SetTimer(0.2f);
		//PlayAnimation("Attack", false);
	}
}

void Player::MoveWithCollision(float elapsedTime,
	float dx,
	float dz,
	const int(&maps)[38][38],
	float value 
)
{
	constexpr float CELL = 2.0f;
	constexpr int OFF_X = 19;
	constexpr int OFF_Z = 19;
	DirectX::XMFLOAT3 next = position;

	next.x += dx * elapsedTime * moveSpeed * value;
	next.z += dz * elapsedTime * moveSpeed * value;

	{
		int nx = (int)std::floor(next.x / CELL) + OFF_X;
		int cz = (int)std::floor(position.z / CELL) + OFF_Z;

		if (maps[cz][nx] == 0)
			position.x = next.x;
	}

	{
		int cx = (int)std::floor(position.x / CELL) + OFF_X;
		int nz = (int)std::floor(next.z / CELL) + OFF_Z;

		if (maps[nz][cx] == 0)
			position.z = next.z;
	}

	col->SetPosition(position);
}

void Player::ChangeWepon()
{
	if (KeyInput::Instance().GetKeyDown(0x31))
	{
		nowWepon = HaveWepon::Sword;
		model = wepons[(int)nowWepon];
	}
	if (KeyInput::Instance().GetKeyDown(0x32))
	{
		nowWepon = HaveWepon::Axe;
		model = wepons[(int)nowWepon];
	}
	if (KeyInput::Instance().GetKeyDown(0x33))
	{
		nowWepon = HaveWepon::Spere;
		model = wepons[(int)nowWepon];
	}
	KeyInput::Instance().Update();
}

bool Player::InputMove(float elapsedTime,const int(&maps)[38][38])
{
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	MoveWithCollision(elapsedTime, moveVec.x, moveVec.z, maps);

	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	if (moveVec.x == 0.0f && moveVec.z == 0.0f&&moveVec.y == 0.0f)
	{
		return false;
	}
	return true;
}

DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	//入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//移動ベクトルはXZ平面に水平なベクトルになるようにする
	//カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX*cameraRightX + cameraRightZ*cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		//単位ベクトル化
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX*cameraFrontX + cameraFrontZ*cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}
	//スティックの水平入力値をカメラ右方向に反映し、
	//スティックの垂直入力値をカメラ前方向に反映し、
	//進行ベクトルを計算する
	if (ax == 1.0f)
	{
		int a = 0;
	}
	DirectX::XMFLOAT3 vec;
	vec.x = cameraFrontX * ay + cameraRightX * ax;
	vec.z = cameraFrontZ * ay + cameraRightZ * ax;
	//Ｙ軸は移動しない
	vec.y = 0.0f;
	return vec;
}

void Player::CollisionWeponVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	bool attacking = col->GetIsAttack();

	int enemyCount = enemyManager.GetEnemyCount();
	for (int j = 0; j < enemyCount; ++j)
	{
		Enemy* enemy = enemyManager.GetEnemy(j);

		DirectX::XMFLOAT3 outPosition;
		bool hitNow = false;

		if (attacking)
		{
			hitNow = Collision::CapsuleVsSphere(position, WeponTipPos, 1.0f, enemy->GetPosition(), 1.0f);
		}

		if (hitNow && !enemy->wasHit)
		{
			int damage = 0;

			switch (nowWepon)
			{
			case Player::HaveWepon::Sword:
				damage = swordDamage * (1.0f + (riskGauge[(int)nowWepon] / 100.0f) * 1.2f);
				break;
			case Player::HaveWepon::Axe:
				damage += chargeValue * AxeDamage * (1.0f + (riskGauge[(int)nowWepon] / 100.0f) * 1.2f);
				break;
			case Player::HaveWepon::Spere:
				damage += SpearDamage * (1.0f + (riskGauge[(int)nowWepon] / 100.0f) * 1.2f);
				break;
			default:
				break;
			}
			enemy->ApplyDamage(damage, 0.0f);
			enemy->PlayHitEffect();

			int riskAdd = 0;

			switch (nowWepon)
			{
			case Player::HaveWepon::Sword:
				riskAdd = 3; 
				break;
			case Player::HaveWepon::Axe:
				riskAdd = 6 * chargeValue;
				break;
			case Player::HaveWepon::Spere:
				riskAdd = 2;
				break;
			}

			riskGauge[(int)nowWepon] = min(
				riskGauge[(int)nowWepon] + riskAdd,
				maxGauge
			);
		}

		enemy->wasHit = hitNow;
	}
}

void Player::CollisionPlyerVsEnemies()
{
	EnemyManager& enemymanager = EnemyManager::Instance();

	int enemyCount = enemymanager.GetEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemymanager.GetEnemy(i);

		DirectX::XMFLOAT3 outPosition;
		/*if (Collision::IntersectSphereVsSpgere(position, radius, enemy->GetPosition(), enemy->GetRadius(), outPosition))
		{
			enemy->SetPosition(outPosition);
		}*/
		if (Collision::IntersectCylinderVsCylinder(position,radius,height,enemy->GetPosition(),enemy->GetRadius(),enemy->GetHeight(),outPosition))
		{
			//enemy->SetPosition(outPosition);
			if (position.y > enemy->GetPosition().y)
			{
				velocity.y = 6;
				enemy->ApplyDamage(1, 0.5f);
			}
			else
			{
				enemy->SetPosition(outPosition);
			}
		}
	}
}

void Player::DrawDebugGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		ImGui::DragInt("HP", &health);

		int b = (int)nowWepon;
		ImGui::DragInt("wepon", &b);

		ImGui::DragInt("SwordRisk", &riskGauge[0]);
		ImGui::DragInt("AxeRisk", &riskGauge[1]);
		ImGui::DragInt("SpareRisk", &riskGauge[2]);

		ImGui::InputFloat("Velocity", &moveVecX);
		ImGui::InputFloat("Velocity", &moveVecZ);

		ImGui::End();
	}
}