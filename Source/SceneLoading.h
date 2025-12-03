#pragma once

#include "System/Sprite.h"
#include "Scene.h"
#include <thread>

//ÉçÅ[Éh
class SceneLoading : public Scene
{
public:
	SceneLoading(Scene* nextScene) : nextScene(nextScene){}
	~SceneLoading() {}

	void Initialize() override;
	void Finalize() override;
	void Update(float elapsedTime) override;
	void Render() override;
	void DrawGUI() override;

private:
	std::unique_ptr<Sprite> sprite = nullptr;
	float angle = 0.0f;
	Scene* nextScene = nullptr;
	std::thread* thread = nullptr;

	static void LoadingThread(SceneLoading* scene);
};