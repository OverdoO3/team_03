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
#include "DamageDrawManager.h"
#include "DirectXCommon.h"

void Player::Initialize()
{
	wepons[0] = std::make_unique<Model>("Data/Model/Player/sword_model.mdl");
	wepons[1] = std::make_unique<Model>("Data/Model/Player/axe_model.mdl");
	wepons[2] = std::make_unique<Model>("Data/Model/Player/spear_model.mdl");

	model = wepons[(int)nowWepon];
	WeponTrailEffect= std::make_unique<Effect>("Data/Effect/wepon.efk");
	riskAura = std::make_unique<Effect>("Data/Effect/risk_aura/risk_aura.efk");
	SpearDash = std::make_unique<Effect>("Data/Effect/spear_dash/spear_dash.efk");
	SwordSlash = std::make_unique<Effect>("Data/Effect/sword_slash/sword_slash.efk");

	hitSE.reset(Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav"));

	dirModel = std::make_unique<Dir>();

	plane = std::make_unique<Plane>();

	scale.x = scale.y = scale.z = 0.012f;

	weponCol = std::make_unique<Wepon>();

	position = { 5,0,3 };

	health = 100;
	attackTimer = 0.0f;

	state = State::Idle;
	PlayAnimation("idle", true);

	sprHP= std::make_unique<Sprite>("Data/Sprite/play_HP_UI.png");
	sprtower= std::make_unique<Sprite>("Data/Sprite/tower_HP_UI.png");
	sprRisuku = std::make_unique<Sprite>("Data/Sprite/risk_UI.png");
	sprwepon = std::make_unique<Sprite>("Data/Sprite/wepon_UI.png");
	sprnumber = std::make_unique<Sprite>("Data/Sprite/number_UI.png");

	isDead = false;
}

void Player::Finalize()
{
}

void Player::Update(float elapsedTime, const int(&maps)[WIDTH][HEIGHT])
{
	if (hitStopTimer > 0.0f)
	{
		hitStopTimer -= elapsedTime;
		return;
	}

	if (health <= 0&&isDead == false)
	{
		respawnTimer = 5.0f;
		riskGauge[0] = 0;
		riskGauge[1] = 0;
		riskGauge[2] = 0;
		isDead = true;
	}
	if (respawnTimer <= 0.0f&&isDead == true)
	{
		health = 100;
		isDead = false;
	}
	else
	{
		respawnTimer -= elapsedTime;
	}

	if (health <= 0)
	{
		health = 0;
	}

	if (!isDead)
	{
		//UpdateVelocity(elapsedTime);

		CollisionPlyerVsEnemies();

		dirModel->Update(position);


		riskAura->SetPosition(riskAuraHandle, position);

		UpdateWeponCollisionFromMotion();

		if (riskGauge[(int)nowWepon] > 75)
		{
			if (riskAuraHandle < 0)riskAuraHandle = riskAura->Play(position);
		}
		else
		{
			riskAura->Stop(riskAuraHandle);
			riskAuraHandle = -1;
		}

		tim += elapsedTime;

		if (tim > 1)
		{
			if (nowWepon == HaveWepon::Sword)
			{
				riskGauge[1] -= elapsedTime;
				riskGauge[2] -= elapsedTime;
			}
			if (nowWepon == HaveWepon::Axe)
			{
				riskGauge[0] -= elapsedTime;
				riskGauge[2] -= elapsedTime;
			}
			if (nowWepon == HaveWepon::Spere)
			{
				riskGauge[0] -= elapsedTime;
				riskGauge[1] -= elapsedTime;
			}
			tim = 0;
		}

		ChangeWepon();

		switch (state)
		{
		case State::Idle:
		{
			switch (nowWepon)
			{
			case HaveWepon::Axe:
				InputCharge(elapsedTime);
				break;
			case HaveWepon::Spere:
				InputRush(elapsedTime, maps);
				break;
			case HaveWepon::Sword:
				InputChain(elapsedTime);
				break;
			default:
				break;
			}

			if (!isChargeRush)
			{
				if (InputMove(elapsedTime, maps))
				{
					state = State::Run;
					PlayAnimation("walk", true);
				}
			}

			InputAvoid();
			InputAttack();
			break;
		}
		case State::Run:
		{
			if (!InputMove(elapsedTime, maps))
			{
				state = State::Idle;
				PlayAnimation("idle", true);
			}

			{
				switch (nowWepon)
				{
				case HaveWepon::Axe:
					InputCharge(elapsedTime);
					break;
				case HaveWepon::Spere:
					InputRush(elapsedTime, maps);
					break;
				case HaveWepon::Sword:
					InputChain(elapsedTime);
					break;
				default:
					break;
				}
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
			weponCol->SetIsAttack(false);
			if (avoidTimer <= 0.0f)
			{
				velocity.x = 0;
				velocity.z = 0;
				chargeValue = 1.0f;
				chargeTime = 0.0f;
				chargestil = 1.0f;

				rushDist = 0.0f;
				isChargeRush = false;
				state = State::Idle;
				PlayAnimation("idle", true);
			}
			break;
		}

		case State::Attack:
		{
			attackTimer += elapsedTime;
			if (animationPlaying)
			{
				if (attackTimer > 0.2f)
				{
					attackTimer = 0.0f;
					weponCol->SetTimer(0.2f);
					weponCol->SetIsAttack(true);
				}
			}
			else
			{
				weponCol->SetIsAttack(false);
				state = State::Idle;
				PlayAnimation("idle", true);
				break;
			}
			InputAvoid();
			InputAttack();
			InputRush(elapsedTime, maps);

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
			SpearDash->SetPosition(SpearDashHandle, position);
			weponCol->SetRadius(1.0f);

			if (rushTimer <= 0.0f)
			{
				state = State::Idle;
				PlayAnimation("idle", true);

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
				weponCol->SetRadius(1.0f);
				state = State::Idle;
				PlayAnimation("idle", true);
			}
		}
		}
		weponCol->Update(elapsedTime);
		plane->SetType((int)nowWepon);
		switch (nowWepon)
		{
		case Player::HaveWepon::Sword:
			break;
		case Player::HaveWepon::Axe:
			plane->Update(elapsedTime, position, chargeTime, angle.y);
			break;
		case Player::HaveWepon::Spere:
			plane->Update(elapsedTime, position, rushDist, angle.y);
			break;
		default:
			break;
		}
		// トランスフォーム更
		model->UpdateTransform();

		CollisionWeponVsEnemies();

		UpdateTransform();

		UpdateAnimation(elapsedTime);

		invincibleTime -= elapsedTime;

		prevState = state;
	}
}

void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	if (!isDead)
	{
		renderer->Render(rc, transform, model.get(), ShaderId::Lambert);;
		dirModel->Render(rc, renderer);
		plane->Render(rc, renderer);
	}
	
}

void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスの関数呼び出し
	Character::RenderDebugPrimitive(rc, renderer);

	renderer->RenderSphere(rc, WeponRootPos, 1.0f, { 1,1,1,1 });
	renderer->RenderSphere(rc, WeponTipPos, 1.0f, { 0,0,1,1 });
}

void Player::RenderUI(const RenderContext& rc)
{
	float s = 1.5;
	sprHP->Render(rc, 100, 900, 0, 376*s, 40*s, 0, 0, 376, 40, 0, 1, 1, 1, 1);
	sprHP->Render(rc, 100, 900, 0, 3.76 * health*s, 40*s, 0, 40, 3.76 * health, 40, 0, 1, 1, 1, 1);
	sprtower->Render(rc, 1750, 650,          0, 80, 300    , 0, 0, 80, 300,     0, 1, 1, 1, 1);
	sprtower->Render(rc, 1750, 950- 3 * 100, 0, 80, 3 * 100, 80,0, 80, 3 * 100, 0, 1, 1, 1, 1);


	sprRisuku->Render(rc, 100, 650, 0, 150*s, 140*s, 750, 0, 150, 140, 0, 1, 1, 1, 1);

	sprwepon->Render(rc, 100, 340, 0, 80*s, 80*s, 0, 0, 80, 80, 0, 1, 1, 1, 1);
	sprwepon->Render(rc, 100, 460, 0, 80*s, 80*s, 0, 80, 80, 80, 0, 1, 1, 1, 1);
	sprwepon->Render(rc, 100, 580, 0, 80*s, 80*s, 0, 160, 80, 80, 0, 1, 1, 1, 1);
	for (int i = 0, t = 0;i < 2;i++)
	{
		sprnumber->Render(rc, 300 - i * 140, 50, 0, 70, 70, (t % 60) % 10 * 100, 0, 100, 100, 0, 1, 1, 1, 1);
		sprnumber->Render(rc, 230 - i * 140, 50, 0, 70, 70, (t % 60) / 10 * 100, 0, 100, 100, 0, 1, 1, 1, 1);
		t = t / 60;
	}
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

		state = State::Attack;
		PlayAnimation("attack", false);
	}
}

void Player::InputAvoid()
{
	if (avoidTimer <= 0.0f&&KeyInput::Instance().GetKeyDown(VK_LSHIFT))
	{
		avoidTimer = 0.2f;
		state = State::Avoid;
		avoidVec = GetMoveVec();
		PlayAnimation("step", false);
	}
}

void Player::InputRush(float elapsedTime, const int(&maps)[WIDTH][HEIGHT])
{
	//右クリック押している間向きをカーソルの方向にするやーつ

	DirectX::XMFLOAT3 vec{};
	if (KeyInput::Instance().GetKeyHold(VK_RBUTTON))
	{
		weponCol->SetIsAttack(false);
		PlayAnimation("charge", false);
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
		rushDist += elapsedTime;
		if (rushDistMax < rushDist)
		{
			rushDist = rushDistMax;
		}

		isChargeRush = true;
	}

	// 離した瞬間：突進
	if (KeyInput::Instance().GetKeyUp(VK_RBUTTON))
	{
		state = State::Rush;
		rushTimer = rushTime * rushDist;
		rushTimer /= 2;
		isChargeRush = false;
		weponCol->SetIsAttack(true);
		weponCol->SetTimer(rushTimer);

		SpearDashHandle = SpearDash->Play(position);
		DirectX::XMFLOAT3 ang = angle;
		ang.y -= DirectX::XM_PI;
		SpearDash->SetAngle(SpearDashHandle, ang);
		SpearDash->SetScale(SpearDashHandle, { rushDist * 0.5f,1.0f,1.0f });
		PlayAnimation("chargeattack", false);
	}
}

void Player::InputCharge(float elapsedTime)
{
	//右クリック押している間向きをカーソルの方向にするやーつ
	DirectX::XMFLOAT3 vec{};
	if (KeyInput::Instance().GetKeyHold(VK_RBUTTON))
	{
		state = State::Charge;
		PlayAnimation("charge",false);
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
		weponCol->SetIsCharge(true);
		weponCol->SetRadius(chargeTime * 1.0f);
		weponCol->SetAngle({ 0,0,0 });
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
		weponCol->SetIsCharge(false);
		weponCol->SetIsAttack(true);
		weponCol->SetTimer(0.2f);
		PlayAnimation("chargeattack", false);
	}
}

void Player::InputChain(float elapsedTime)
{
	//右クリック押している間向きをカーソルの方向にするやーつ
	DirectX::XMFLOAT3 vec{};
	if (KeyInput::Instance().GetKeyHold(VK_RBUTTON))
	{
		state = State::Charge;
		PlayAnimation("charge", false);
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
		weponCol->SetIsCharge(true);
		weponCol->SetRadius(chargeTime * 1.0f);
		weponCol->SetAngle(angle);
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
		weponCol->SetIsCharge(false);
		weponCol->SetIsAttack(true);
		weponCol->SetPosition(position);
		weponCol->SetTimer(0.2f * chargeValue);
		PlayAnimation("chargeattack", false);
	}
}

void Player::MoveWithCollision(float elapsedTime,
	float dx,
	float dz,
	const int(&maps)[WIDTH][HEIGHT],
	float value)
{
	constexpr float CELL = 2.0f;
	constexpr int OFF_X = WIDTH / 2;
	constexpr int OFF_Z = HEIGHT / 2;
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

	weponCol->SetPosition(position);
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

void Player::UpdateWeponCollisionFromMotion()
{
	
	switch (nowWepon)
	{
	case Player::HaveWepon::Sword:
		for (auto& node : model->GetNodes())
		{
			DirectX::XMMATRIX local = DirectX::XMLoadFloat4x4(&node.globalTransform);
			DirectX::XMMATRIX playerWorld = DirectX::XMLoadFloat4x4(&transform);
			DirectX::XMMATRIX worldMat = local * playerWorld;
			if (strcmp(node.name, "joint3") == 0)
			{
				WeponTipPos.x = worldMat.r[3].m128_f32[0];
				WeponTipPos.y = worldMat.r[3].m128_f32[1];
				WeponTipPos.z = worldMat.r[3].m128_f32[2];
			}

			if (strcmp(node.name, "pasted__L_hand") == 0)
			{
				WeponRootPos.x = worldMat.r[3].m128_f32[0];
				WeponRootPos.y = worldMat.r[3].m128_f32[1];
				WeponRootPos.z = worldMat.r[3].m128_f32[2];
			}
		}
		break;
	case Player::HaveWepon::Axe:
		for (auto& node : model->GetNodes())
		{
			DirectX::XMMATRIX local = DirectX::XMLoadFloat4x4(&node.globalTransform);
			DirectX::XMMATRIX playerWorld = DirectX::XMLoadFloat4x4(&transform);
			DirectX::XMMATRIX worldMat = local * playerWorld;
			if (strcmp(node.name, "joint3") == 0)
			{
				WeponTipPos.x = worldMat.r[3].m128_f32[0];
				WeponTipPos.y = worldMat.r[3].m128_f32[1];
				WeponTipPos.z = worldMat.r[3].m128_f32[2];
			}

			if (strcmp(node.name, "pasted__L_hand") == 0)
			{
				WeponRootPos.x = worldMat.r[3].m128_f32[0];
				WeponRootPos.y = worldMat.r[3].m128_f32[1];
				WeponRootPos.z = worldMat.r[3].m128_f32[2];
			}
		}
		break;
	case Player::HaveWepon::Spere:
		for (auto& node : model->GetNodes())
		{
			DirectX::XMMATRIX local = DirectX::XMLoadFloat4x4(&node.globalTransform);
			DirectX::XMMATRIX playerWorld = DirectX::XMLoadFloat4x4(&transform);
			DirectX::XMMATRIX worldMat = local * playerWorld;
			if (strcmp(node.name,"joint3") == 0)
			{
				WeponTipPos.x = worldMat.r[3].m128_f32[0];
				WeponTipPos.y = worldMat.r[3].m128_f32[1];
				WeponTipPos.z = worldMat.r[3].m128_f32[2];
			}

			if (strcmp(node.name, "pasted__pasted__L_hand") == 0)
			{
				WeponRootPos.x = worldMat.r[3].m128_f32[0];
				WeponRootPos.y = worldMat.r[3].m128_f32[1];
				WeponRootPos.z = worldMat.r[3].m128_f32[2];
			}
		}
		break;
	default:
		break;
	}
	WeponTipPos.y = 1.0f;
}

bool Player::InputMove(float elapsedTime,const int(&maps)[WIDTH][HEIGHT])
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

	bool attacking = weponCol->GetIsAttack();

	int enemyCount = enemyManager.GetEnemyCount();
	for (int j = 0; j < enemyCount; ++j)
	{
		Enemy* enemy = enemyManager.GetEnemy(j);

		DirectX::XMFLOAT3 outPosition;
		bool hitNow = false;

		if (attacking)
		{
			hitNow = Collision::CapsuleVsSphere(WeponRootPos, WeponTipPos, 1.0f, enemy->GetPosition(), 1.0f);
		}

		if (hitNow && !enemy->wasHit)
		{
			int damage = 0;

			switch (nowWepon)
			{
			case Player::HaveWepon::Sword:
				damage += swordDamage * (1.0f + (riskGauge[(int)nowWepon] / 100.0f) * 1.2f);
				break;
			case Player::HaveWepon::Axe:
				damage += chargeValue * AxeDamage * (1.0f + (riskGauge[(int)nowWepon] / 100.0f) * 1.2f);
				break;
			case Player::HaveWepon::Spere:
				damage += SpearDamage * (1.0f + (riskGauge[(int)nowWepon] / 100.0f) * 3.0f);
				break;
			default:
				break;
			}

			damage += rand() % 5;
			// スクリーンサイズ取得
			float screenWidth = Graphics::Instance().GetScreenWidth();
			float screenHeight = Graphics::Instance().GetScreenHeight();

			DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
			DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&Camera::Instance().GetProjection());
			DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

			DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(a,
				0, 0, screenWidth, screenHeight,
				0, 1.0f, Projection, View, World);

			DirectX::XMFLOAT2 screenPosition;
			DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

			DamegeDrawManager::Instance().makeTexts(damage, screenPosition, 0.4f, { 1,1,1,1 }, 0.4f);

			enemy->ApplyDamage(damage, 2.0f);
			enemy->PlayHitEffect();

			int riskAdd = 0;

			switch (nowWepon)
			{
			case Player::HaveWepon::Sword:
				riskAdd = 3;
				hitStopTimer = 0.02f;
				break;
			case Player::HaveWepon::Axe:
				riskAdd = 6 * chargeValue;
				if (chargeValue == 1)
				{
					hitStopTimer = 0.05f;
				}
				else
				{
					hitStopTimer = 0.08f;
				}
				break;
			case Player::HaveWepon::Spere:
				riskAdd = 2;
				hitStopTimer = 0.01f;
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
	/*ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		ImGui::DragInt("HP", &health);
		ImGui::InputFloat("Angle", &angle.y);

		int b = (int)nowWepon;
		ImGui::InputInt("wepon", &b);

		ImGui::DragFloat("rushDist", &rushDist);

		int a = (int)state;
		ImGui::InputInt("State", &a);

		ImGui::DragInt("SwordRisk", &riskGauge[0]);
		ImGui::DragInt("AxeRisk", &riskGauge[1]);
		ImGui::DragInt("SpareRisk", &riskGauge[2]);

		ImGui::DragFloat3("WeponTipPos", &WeponTipPos.x);

		ImGui::End();
	}*/
}