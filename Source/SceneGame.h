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
	static constexpr int WIDTH = 60;
	static constexpr int HEIGHT = 60;

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

	float gameTimer = 0.0f;

	int maps[WIDTH][HEIGHT];

	std::unique_ptr<CameraController> cameraController = nullptr;

	std::unique_ptr<Sprite> towerBreakSpr;
	std::unique_ptr<Sprite> blackSpr;

	std::unique_ptr<Sprite> fadeSpr;
	std::unique_ptr<Sprite> resultSpr;

	std::unique_ptr<Sprite> NumberSpr;

	XMFLOAT2 resultKillPos = { 650,340 };
	XMFLOAT2 resultWavePos = { 650,600 };

	std::unique_ptr<Effect> explosion;
	Effekseer::Handle expHandle;

	int killCount = 0;
	int waveCount = 0;

	bool isAlive = true;
	bool isResult = false;
	bool isResultShow = false;
	float resultTimer = 1.0f;

	DirectX::XMFLOAT3 Lerp(
		const DirectX::XMFLOAT3& a,
		const DirectX::XMFLOAT3& b,
		float t)
	{
		return {
			a.x + (b.x - a.x) * t,
			a.y + (b.y - a.y) * t,
			a.z + (b.z - a.z) * t
		};
	}
};
