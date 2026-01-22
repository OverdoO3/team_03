#pragma once
#include "System/Sprite.h"
#include "Scene.h"

class SceneTutorial : public Scene
{
public:
	SceneTutorial() {}
	~SceneTutorial() override {}

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
	std::unique_ptr<Sprite> spr = nullptr;

	int c = 0;
};