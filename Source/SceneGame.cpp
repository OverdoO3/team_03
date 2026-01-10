#include "System/Graphics.h"
#include "Camera.h"
#include "SceneGame.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "Player.h"
#include "EffectManager.h"
#include "nlohmann/json.hpp"
#include "fstream"
using json = nlohmann::json;
// 初期化
void SceneGame::Initialize()
{
	stage = std::make_unique<Stage>();
	pathfinding = std::make_unique<Pathfinding>();
	pathfinding->Initialize(stage.get());
	Player::Instance().Initialize();
	//player = new Player();

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

	int index = 0;
	std::ifstream file("map.json");
	if (file)
	{
		json data = json::parse(file);
		for (int i = 0;i < data["Width"];i++)
		{
			for (int j = 0;j < data["Height"];j++)
			{
				stage->IsWalkable(i, j);
				maps[i][j] = data["cells"][index++].get<int>();
			}
		}
	}

	//エネミー初期化
	EnemyManager& enemymanager = EnemyManager::Instance();
	//for (int i = 0; i < 2;++i)
	{
		EnemySlime* slime = new EnemySlime(stage.get(), pathfinding.get());
		slime->InitializeEnemy(stage.get(), pathfinding.get());
		slime->SetPosition(DirectX::XMFLOAT3(1 * 2.0f, 0, 5));
		slime->SetReady(true);
		enemymanager.Register(slime);
	}

	Player::Instance().setStage(stage.get());
}
// 終了化
void SceneGame::Finalize()
{
	Player::Instance().Finalize();
	EnemyManager& enemymanager = EnemyManager::Instance();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	//DirectX::XMFLOAT3 target = player->GetPosition();]
	DirectX::XMFLOAT3 target = Player::Instance().GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);
	stage->Update(elapsedTime);
	Player::Instance().Update(elapsedTime,maps);
	EnemyManager& enemymanager = EnemyManager::Instance();
	//enemymanager.Update(elapsedTime,Player::Instance().GetPosition());
	enemymanager.Update(elapsedTime,*stage->GetTower());
	EffectManager::Instance().Update(elapsedTime);
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
		//player->Render(rc, modelRenderer);
		//ステージ描画
		stage->Render(rc, modelRenderer);
		Player::Instance().Render(rc, modelRenderer);
		EnemyManager::Instance().Render(rc,modelRenderer);
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3Dデバッグ描画
	{
		
		//player->RenderDebugPrimitive(rc, shapeRenderer);
		Player::Instance().RenderDebugPrimitive(rc, shapeRenderer);
		//エネミーデバッグプリミティブ
		EnemyManager::Instance().RenderDebugPrimitive(rc,shapeRenderer);

		stage->DebugDrawGrid(rc, shapeRenderer,modelRenderer);
	}

	// 2Dスプライト描画
	{

	}
}

// GUI描画
void SceneGame::DrawGUI()
{
	//player->DrawDebugGUI();
	Player::Instance().DrawDebugGUI();
	stage->DrawDebugGUI();
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 200), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	/*if (ImGui::Begin("Enemy", nullptr, ImGuiWindowFlags_None))
	{
		ImGui::End();
	}*/
}
