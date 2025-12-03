#pragma once

#include "Stage.h"
#include "Scene.h"
#include "CameraController.h"
#include <wrl.h>
#include <windows.h>

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

	//std::unique_ptr<Player> player = nullptr;

	std::unique_ptr<CameraController> cameraController = nullptr;
};
