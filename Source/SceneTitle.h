#pragma once
#include "System/Sprite.h"
#include "Scene.h"
#include "tower.h"
#include "CameraController.h"
#include "Camera.h"
#include "Stage.h"

class SceneTitle : public Scene
{
public:
	SceneTitle() {}
	~SceneTitle() override {}

	//‰Šú‰»
	void Initialize() override;

	//I—¹‰»
	void Finalize() override;

	//XVˆ—
	void Update(float elapsedTime)override;

	//•`‰æˆ—
	void Render() override;

	//GUI•`‰æ
	void DrawGUI() override;
private:
	std::unique_ptr<Sprite> sprite = nullptr;
	std::unique_ptr<Sprite> sprUI = nullptr;
	std::unique_ptr<Sprite> spr = nullptr;

	std::unique_ptr<CameraController> cameraController = nullptr;

	int mousec=0;

	std::unique_ptr<Stage>stage = nullptr;

	DirectX::XMFLOAT3 pos = {-12,-2,4};
	DirectX::XMFLOAT3 ang = { 0,19.2,0 };
	DirectX::XMFLOAT3 sca = { 0.03f,0.03f,0.03f };

	DirectX::XMFLOAT3 camera_eye = {0, 7, 18};
	DirectX::XMFLOAT3 camera_target = {-6, 2, 4};
};