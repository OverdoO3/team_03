#pragma once
#include "System/Sprite.h"
#include "Scene.h"
#include "tower.h"
#include "CameraController.h"
#include "Camera.h"

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

	std::unique_ptr<Tower>tower = nullptr;

	std::unique_ptr<CameraController> cameraController = nullptr;

	int mousec=0;
};