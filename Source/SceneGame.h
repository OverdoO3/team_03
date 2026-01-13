#pragma once

#include "Stage.h"
#include "Scene.h"
#include "CameraController.h"
#include <wrl.h>
#include <windows.h>
#include "Pathfinding.h"
#include "WaveManager.h"
#include "System/Sprite.h"

// ゲームシーン
class SceneGame : public Scene
{
public:
	SceneGame() {};
	~SceneGame()  override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

	// GUI描画
	void DrawGUI() override;
private:
	std::unique_ptr<Stage> stage = nullptr;
	std::unique_ptr<Pathfinding> pathfinding = nullptr;

	Enemy* dummy;

	float gameTimer = 0.0f;

	int maps[38][38];

	std::unique_ptr<CameraController> cameraController = nullptr;

	DirectX::XMFLOAT3 testPos = { 720,320,1 };
	float testAngle = 0;

	std::unique_ptr<Sprite> test = nullptr;
};
