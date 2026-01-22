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

void Player::Initialize()
{
	model = std::make_unique<Model>("Data/Model/Jammo/Jammo.mdl");
	hitEffect = std::make_unique<Effect>("Data/Effect/Hit.efk");
	trailEffect= std::make_unique<Effect>("Data/Effect/trail_demo.efk");
	hitSE.reset(Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav"));
	scale.x = scale.y = scale.z = 0.01f;
	trailEffect->SetPosition(trailHandle, position);
	trailHandle = trailEffect->Play({ 0,5.0f,0 }, 0.5f);

	col = std::make_unique<OnCollisionWepon>();
}

void Player::Finalize()
{
}

void Player::Update(float elapsedTime)
{
	InputJump();

	

	UpdateVelocity(elapsedTime);

	projectileManager.Update(elapsedTime);

	CollisionPlyerVsEnemies();

	//InputMove(elapsedTime);

	CollisionWeponVsEnemies();

	trailEffect->SetPosition(trailHandle, position);

	col->Update(elapsedTime);

	switch (state)
	{
	case State::Idle:
	{
		if (InputMove(elapsedTime))
		{
			state = State::Run;
			PlayAnimation("Running", true);
		}

		InputProjectile();
		break;
	}

	case State::Run:
	{
		if (!InputMove(elapsedTime))
		{
			state = State::Idle;
			PlayAnimation("Idle", true);
		}

		InputProjectile();
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
	}
	}

	// トランスフォーム更

	UpdateTransform();

	UpdateAnimation(elapsedTime);

}

void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model.get(), ShaderId::Lambert);

	projectileManager.Render(rc, renderer);
}

void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスの関数呼び出し
	Character::RenderDebugPrimitive(rc, renderer);
	//弾丸デバッグプリミティブ描画
	projectileManager.RenderDebugPrimitive(rc, renderer);

	col->RenderDebugPrimitive(rc, renderer);
}

void Player::InputProjectile()
{
	//発射
	bool isPressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

	//左クリック押している間向きをカーソルの方向にするやーつ
	
	//if (GetAsyncKeyState(VK_LBUTTON))
	//{
	//	DirectX::XMFLOAT3 curPosS = Screen::GetScreenCursorWorld(&Camera::Instance(),0);
	//	DirectX::XMFLOAT3 curPosE = Screen::GetScreenCursorWorld(&Camera::Instance(),1.0f);
	//	DirectX::XMFLOAT3 hitPosition;
	//	DirectX::XMFLOAT3 hitNormal;
	//	Hit::RayCast(curPosS, curPosE, stage->getTransform(), stage->getModel(), hitPosition, hitNormal);

	//	DirectX::XMFLOAT3 vec = { position.x - hitPosition.x,position.y - hitPosition.y,position.z - hitPosition.z };
	//	// XZ 平面に投影
	//	float dx = vec.x;
	//	float dz = vec.z;

	//	// Y軸まわりの回転角
	//	float angles = atan2f(-dx, -dz); // ※向きによって順番調整

	//	angle.y = angles;
	//	UpdateTransform();
	//}

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
		//発射
		col->SetPosition({ pos.x + dir.x ,pos.y + dir.y ,pos.z + dir.z });
		col->SetIsAttack(true);
		col->SetTimer(0.2f);

		state = State::Attack;
		PlayAnimation("Attack", false);
	}

	//玉撃ち置いとくだけ
	//if (!isPressed && wasPressed)
	//{
	//	DirectX::XMFLOAT3 dir;
	//	dir.x = sinf(angle.y);
	//	dir.y = 0.0f;
	//	dir.z = cosf(angle.y);
	//	//発射位置（プレイヤーの腰当たり）
	//	DirectX::XMFLOAT3 pos;
	//	pos.x = position.x;
	//	pos.y = position.y + 0.8f;
	//	pos.z = position.z;
	//	//発射
	//	ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
	//	projectile->Launch(dir, pos);
	//}
	wasPressed = isPressed;
}

void Player::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		if (jumpLimit > jumpCount)
		{
			Jump(jumpSpeed);
			jumpCount++;
		}
	}
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


void Player::OnLanding()
{
	jumpCount = 0;
}

void Player::DrawDebugGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		if(ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputFloat3("Positon", &position.x);

			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);

			ImGui::InputFloat3("Scale", &scale.x);
		}
		ImGui::End();
	}
}
bool Player::InputMove(float elapsedTime)
{
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);

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

	int enemyCount = enemyManager.GetEnemyCount();
	for (int j = 0; j < enemyCount; ++j)
	{
		Enemy* enemy = enemyManager.GetEnemy(j);
		//衝突処理
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsCylinder(
			col->GetPosition(),
			col->GetRadius(),
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition)&&col->GetIsAttack())
		{
			if (enemy->ApplyDamage(1, 0.5f))
			{
				DirectX::XMFLOAT3 e = enemy->GetPosition();
				e.y += 1.0f;
				hitEffect->Play(e, 2.0f);

				hitSE->Play(false);
			}
		}
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
