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
	model = std::make_unique<Model>("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
	hitEffect = std::make_unique<Effect>("Data/Effect/Hit.efk");
	trailEffect= std::make_unique<Effect>("Data/Effect/trail_demo.efk");
	hitSE.reset(Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav"));
	scale.x = scale.y = scale.z = 0.01f;
	trailEffect->SetPosition(trailHandle, position);
	trailHandle = trailEffect->Play({ 0,5.0f,0 }, 0.5f);
	int sakai;
}

void Player::Finalize()
{
}

void Player::Update(float elapsedTime)
{
	InputJump();

	InputProjectile();

	UpdateVelocity(elapsedTime);

	projectileManager.Update(elapsedTime);

	CollisionPlyerVsEnemies();

	InputMove(elapsedTime);

	UpdateTransform();

	model->UpdateTransform();

	CollisionProjectilesVsEnemies();

	trailEffect->SetPosition(trailHandle, position);
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
}

void Player::InputProjectile()
{
	//発射
	bool isPressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

	if (GetAsyncKeyState(VK_LBUTTON))
	{
		DirectX::XMFLOAT3 curPosS = Screen::GetScreenCursorWorld(&Camera::Instance(),0);
		DirectX::XMFLOAT3 curPosE = Screen::GetScreenCursorWorld(&Camera::Instance(),1.0f);
		DirectX::XMFLOAT3 hitPosition;
		DirectX::XMFLOAT3 hitNormal;
		Hit::RayCast(curPosS, curPosE, stage->getTransform(), stage->getModel(), hitPosition, hitNormal);

		DirectX::XMFLOAT3 vec = { position.x - hitPosition.x,position.y - hitPosition.y,position.z - hitPosition.z };
		// XZ 平面に投影
		float dx = vec.x;
		float dz = vec.z;

		// Y軸まわりの回転角
		float angles = atan2f(-dx, -dz); // ※向きによって順番調整

		angle.y = angles;
		UpdateTransform();
	}

	if (!isPressed && wasPressed)
	{
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);
		//発射位置（プレイヤーの腰当たり）
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + 0.8f;
		pos.z = position.z;
		//発射
		ProjectileStraight * projectile = new ProjectileStraight(&projectileManager);
		projectile->Launch(dir, pos);
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
void Player::InputMove(float elapsedTime)
{
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);

	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
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

void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//すべての弾とすべての敵を総当たり
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);
		for (int j = 0; j < enemyCount; ++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			//衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				if (enemy->ApplyDamage(1, 0.5f))
				{
					//吹き飛ばし
					DirectX::XMFLOAT3 impluse;
					impluse.x = 10*projectile->GetDirection().x;
					impluse.y = 10.0f;
				    impluse.z = 10*projectile->GetDirection().z;
					enemy->AddImpulse(impluse);

					DirectX::XMFLOAT3 e = enemy->GetPosition();
					e.y += 1.0f;
					hitEffect->Play(e,2.0f);

					hitSE->Play(false);

					//破棄
					projectile->Destroy();
				}
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
