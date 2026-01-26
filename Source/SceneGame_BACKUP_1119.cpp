#include "System/Graphics.h"
#include "System/Input.h"
#include "Camera.h"
#include "SceneGame.h"
#include "EnemyManager.h"
#include "Player.h"
#include "EffectManager.h"
<<<<<<< HEAD
#include "nlohmann/json.hpp"
#include "fstream"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneTitle.h"
#include "DamageDrawManager.h"
#include <algorithm>
#include "KeyInput.h"

using json = nlohmann::json;
=======
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneTitle.h"
>>>>>>> origin/main
// 初期化
void SceneGame::Initialize()
{
	stage = std::make_unique<Stage>();
	pathfinding = std::make_unique<Pathfinding>();
	pathfinding->Initialize(stage.get());
	Player::Instance().Initialize();
	EnemyManager::Instance().Initialize();

	EffectManager::Instance().Initialize();
	WaveManager::Instance().Initialize();

	//カメラしょきか
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),//視点
		DirectX::XMFLOAT3(0, 0, 0),   //注視店
		DirectX::XMFLOAT3(0, 1, 0)    //上方向
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),//視野角
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),//アス比
		0.1f,//クリップ近
		1000.0f//クリップ遠
	);
	cameraController = std::make_unique<CameraController>();
	
	static int count = 0;

	towerBreakSpr = std::make_unique<Sprite>("Data/Sprite/tower_break_text.png");
	blackSpr = std::make_unique<Sprite>("Data/Sprite/black.png");
	fadeSpr = std::make_unique<Sprite>("Data/Sprite/black.png");
	resultSpr = std::make_unique<Sprite>("Data/Sprite/game_result_backgraund.png");

	NumberSpr = std::make_unique<Sprite>("Data/Sprite/number_UI2.png");

	explosion = std::make_unique<Effect>("Data/Effect/explosion/explosion.efk");

	if (count > 0) return;

	int index = 0;
	std::ifstream file("map.json");
	if (file)
	{
		json data = json::parse(file);
		for (int y = 0;y < data["Width"];y++)
		{
			for (int x = 0;x < data["Height"];x++)
			{
				stage->IsWalkable(y,x);
				maps[y][x] = data["cells"][index++].get<int>();
				if (maps[y][x] == 3)
				{
					DirectX::XMFLOAT3 pos;
					pos = stage->GridToWorld(x, y);
					WaveManager::Instance().MakeSpawnPoint(pos,rand() % 3);
				}
				if (maps[x][y] == 4)
				{
					DirectX::XMFLOAT3 pos;
					pos = stage->GridToWorld(y, x);
					stage->GetNexus()->SetPosition(pos);
				}
			}
		}
	}
	count++;
	Player::Instance().setStage(stage.get());
<<<<<<< HEAD


=======
	sprite = std::make_unique<Sprite>("Data/Sprite/POSE.png");
	sprUI = std::make_unique<Sprite>("Data/Sprite/POSE_UI.png");
	pose = false;
>>>>>>> origin/main
}
// 終了化
void SceneGame::Finalize()
{
	Player::Instance().Finalize();
	EnemyManager::Instance().Finalize();
	explosion->Stop(expHandle);
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
<<<<<<< HEAD
	if (stage->GetNexus()->GetHP() <= 0)
	{
		isAlive = false;
		if (explosion)
		{
			expHandle = explosion->Play(stage->GetNexus()->GetPosition(), true);
		}
	}
	if (!isAlive)
	{
		EffectManager::Instance().Update(elapsedTime);

		float smooth = 1.0f;
		float t = 1.0f - expf(-smooth * elapsedTime);

		DirectX::XMFLOAT3 eye = Camera::Instance().GetEye();

		DirectX::XMFLOAT3 targetEye = stage->GetNexus()->GetPosition();
		targetEye.y += 20.0f;
		targetEye.z += 20.0f;

		eye = Lerp(eye, targetEye, t);

		cameraController->SetManualEye(true);
		cameraController->SetEye(eye);
		cameraController->SetTarget(stage->GetNexus()->GetPosition());
		cameraController->Update(elapsedTime);

		KeyInput::Instance().Update();

		if (KeyInput::Instance().GetKeyDown(VK_LBUTTON))
		{
			isResult = true;
		}
		if (isResult)
		{
			resultTimer -= elapsedTime;
			if (resultTimer < 0.0f)
			{
				isResultShow = true;
			}
			if (isResultShow)
			{
				if (KeyInput::Instance().GetKeyDown(VK_LBUTTON))
				{
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
				}
			}
		}
		return;
	}

=======
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		pose = true;
	}
	if (pose)
	{
		POSE();
		return;
	}
	//DirectX::XMFLOAT3 target = player->GetPosition();]
>>>>>>> origin/main
	DirectX::XMFLOAT3 target = Player::Instance().GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	Player::Instance().Update(elapsedTime, maps);
	if (Player::Instance().GetHitStopTimer() <= 0.0f)
	{
		EnemyManager& enemymanager = EnemyManager::Instance();
		enemymanager.Update(elapsedTime, stage->GetTower(), stage->GetNexus());
	}

	EffectManager::Instance().Update(elapsedTime);
	stage->Update(elapsedTime);
	gameTimer += elapsedTime;
	WaveManager::Instance().Update(elapsedTime, stage.get(), pathfinding.get());
	DamegeDrawManager::Instance().Update(elapsedTime);
}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();

	// 描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f };	// ライト方向（下方向）
	rc.renderState = graphics.GetRenderState();

	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	// 3Dモデル描画
	{
		//ステージ描画
		stage->Render(rc, modelRenderer);
		Player::Instance().Render(rc, modelRenderer);
		EnemyManager::Instance().Render(rc,modelRenderer);
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3Dデバッグ描画
	{
		Player::Instance().RenderDebugPrimitive(rc, shapeRenderer);
		//エネミーデバッグプリミティブ
		EnemyManager::Instance().RenderDebugPrimitive(rc,shapeRenderer);

		stage->DebugDrawGrid(rc, shapeRenderer,modelRenderer);

		WaveManager::Instance().RenderDebugPrimitive(rc, shapeRenderer);
	}

	// 2Dスプライト描画
	{
		DamegeDrawManager::Instance().Render(rc);

		if (!isAlive)
		{
			if (isResultShow)
			{
				blackSpr->Render(rc, 0, 0, 0, 1920, 1080, 0, 1, 1, 1, resultTimer + 0.5f);
				resultSpr->Render(rc, 0, 0, 0, 1920, 1080, 0, 1, 1, 1, 1);
				scoreRender::ScoreRenderDigit_NoHead_Spacing(rc, NumberSpr.get(), WaveManager::Instance().GetWaveCount(), 100, 100, resultKillPos.x, resultKillPos.y, 0, 0, 1, 1, 1.0f);
				scoreRender::ScoreRenderDigit_NoHead_Spacing(rc, NumberSpr.get(), EnemyManager::Instance().GetKillCount(), 100, 100, resultWavePos.x, resultWavePos.y, 0, 0, 1, 1, 1.0f);
			}
			else
			{
				blackSpr->Render(rc, 0, 0, 0, 1920, 1080, 0, 1, 1, 1, 0.5f);
				towerBreakSpr->Render(rc, 0, 0, 0, 1920, 1080, 0, 1, 1, 1, resultTimer);
			}
		}
		Player::Instance().RenderUI(rc);
		if (pose)
		{
			sprite->Render(rc,
				0, 0, 0, 1920, 1080,
				0,
				1, 1, 1, 1);
			sprUI->Render(rc, 100, 900, 0, 400, 100, 800 - 800 * (mousec % 2), 0, 800, 200, 0, 1, 1, 1, 1);
			sprUI->Render(rc, 1400, 900, 0, 400, 100, 800 - 800 * (mousec / 2 % 2), 200, 800, 200, 0, 1, 1, 1, 1);
			
		}
	}
}

// GUI描画
void SceneGame::DrawGUI()
{
	Player::Instance().DrawDebugGUI();
	stage->DrawDebugGUI();
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 200), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("SPR", nullptr, ImGuiWindowFlags_None))
	{
		ImGui::DragFloat2("killPos", &resultKillPos.x);
		ImGui::DragFloat2("wavePos", &resultWavePos.x);

		ImGui::End();
	}
	
	WaveManager::Instance().DebugGUI();
}

void SceneGame::POSE()
{
	

	Mouse& mouse = Input::Instance().GetMouse();
	mousec = 0;
	if (mouse.GetOldPositionY() > 900 && mouse.GetOldPositionY() < 1000)
	{
		if (mouse.GetOldPositionX() > 100 && mouse.GetOldPositionX() < 500)
		{
			mousec = 1;
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				pose = false;
			}
		}
		if (mouse.GetOldPositionX() > 1400 && mouse.GetOldPositionX() < 1800)
		{
			mousec = 2;
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
			}
			
		}
		
	}
}
