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
		InputRush(elapsedTime, maps);
		InputAttack();
		break;
	}

	case State::Run:
	{
		if (!InputMove(elapsedTime,maps))
		{
			state = State::Idle;
			PlayAnimation("Idle", true);
		}
		InputRush(elapsedTime, maps);
		InputAttack();
		moveSpeed = 5.0f;
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
	case State::Special:
	{
		float dx = rushVec.x * rushSpeed * rushDist * elapsedTime;
		float dz = rushVec.z * rushSpeed * rushDist * elapsedTime;

		MoveWithCollision(elapsedTime,dx, dz, maps,true);

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
	//発射
	bool isPressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
	//if (GetAsyncKeyState(VK_LBUTTON))
	if (!isPressed && wasPressed)
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
		//発射位置（プレイヤーの腰当たり）
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
	wasPressed = isPressed;

}

void Player::InputRush(float elapsedTime, const int(&maps)[38][38])
{
	bool isPressedR = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

	//右クリック押している間向きをカーソルの方向にするやーつ

	DirectX::XMFLOAT3 vec{};
	if (isPressedR)
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
	if (!isPressedR && wasPressedR)
	{
		rushTimer = rushTime * rushDist;
		state = State::Special;
		moveSpeed = 5.0f;
		isChargeRush = false;
		col->SetIsAttack(true);
		col->SetTimer(rushTimer);
		//PlayAnimation("Attack", false);
	}
	wasPressedR = isPressedR;
}

// アニメーション再生
void Player::PlayAnimation(int index, bool loop)
{
	animationPlaying = true;
	animationLoop = loop;
	animationIndex = index;
	animationSeconds = 0.0f;
}

void Player::PlayAnimation(const char* name, bool loop)
{
	int index = 0;
	const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
	for (const ModelResource::Animation& animation : animations)
	{
		if (animation.name == name)
		{
			PlayAnimation(index, loop);
			return;
		}
		++index;
	}
}


// アニメーション更新処理
void Player::UpdateAnimation(float elapsedTime)
{
	if (animationPlaying)
	{
		//アニメーション切り替え時のブレンド率を計算
		float blendRate = 1.0f;
		if (animationSeconds < animationBlendSecondsLength)
		{
			blendRate = (animationSeconds / animationBlendSecondsLength);
		}

		std::vector<Model::Node>& nodes = model->GetNodes();

		//アニメーションを取得
		const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
		const ModelResource::Animation& animation = animations.at(animationIndex);

		animationSeconds += elapsedTime;

		if (animationSeconds >= animation.secondsLength)
		{
			if (animationLoop)
			{
				animationSeconds = 0;
			}
			else
			{
				animationPlaying = false;
			}
		}

		const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
		int keyCount = static_cast<int>(keyframes.size());

		for (int keyIndex = 0;keyIndex < keyCount - 1; ++keyIndex)
		{
			//現在の時間がどのキーフレームかを判定する
			const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
			const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
			if (animationSeconds >= keyframe0.seconds && animationSeconds < keyframe1.seconds)
			{
				//再生時間とキーフレームの時間から補完率を算出
				float rate = ((animationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds));

				//すべてのノードの姿勢を計算する
				int nodeCount = static_cast<int>(nodes.size());
				for (int nodeIndex = 0; nodeIndex < nodeCount;++nodeIndex)
				{
					const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
					const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

					//ノード取得
					Model::Node& node = nodes[nodeIndex];

					if (blendRate < 1.0f)
					{
						//現在と次の姿勢を補完
						DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&node.scale);
						DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
						DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&node.rotate);
						DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
						DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&node.translate);
						DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);

						DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, blendRate);
						DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, blendRate);
						DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, blendRate);

						DirectX::XMStoreFloat3(&node.scale, S);
						DirectX::XMStoreFloat4(&node.rotate, R);
						DirectX::XMStoreFloat3(&node.translate, T);
					}
					else
					{
						//前のキーフレームと次のキーフレームの姿勢を補完
						DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scale);
						DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
						DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotate);
						DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
						DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translate);
						DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);

						DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
						DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
						DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);

						DirectX::XMStoreFloat3(&node.scale, S);
						DirectX::XMStoreFloat4(&node.rotate, R);
						DirectX::XMStoreFloat3(&node.translate, T);
					}
				}
			}
		}
	}
	//行列更新
	model->UpdateTransform();
}

void Player::MoveWithCollision(float elapsedTime,
	float dx,
	float dz,
	const int(&maps)[38][38],
	bool isRush 
)
{
	constexpr float CELL = 2.0f;
	constexpr int OFF_X = 19;
	constexpr int OFF_Z = 19;
	DirectX::XMFLOAT3 next = position;
	if (isRush)
	{
		next.x += dx * elapsedTime * moveSpeed * 10;
		next.z += dz * elapsedTime * moveSpeed * 10;
	}
	else
	{
		next.x += dx * elapsedTime * moveSpeed;
		next.z += dz * elapsedTime * moveSpeed;
	}

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
				damage += AxeDamage * (1.0f + (riskGauge[(int)nowWepon] / 100.0f) * 1.2f);
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
				riskAdd = 6;
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

		ImGui::End();
	}
}