#pragma once
#include "System/Sprite.h"
#include "System/Audio.h"

#include "Scene.h"

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
	AudioSource* SE = nullptr;
	AudioSource* BGM = nullptr;

	int mousec=0;
};