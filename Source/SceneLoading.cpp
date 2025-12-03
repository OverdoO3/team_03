#include "System/Graphics.h"
#include "System/Input.h"
#include "SceneLoading.h"
#include "SceneManager.h"

void SceneLoading::Initialize()
{
	sprite = std::make_unique<Sprite>("Data/Sprite/LoadingIcon.png");

	thread = new std::thread(&SceneLoading::LoadingThread, this);
}

void SceneLoading::Finalize()
{
	//スレッド終了化
	if (IsReady() == false)
	{
		thread->join();
	}
}

void SceneLoading::Update(float elapsedTime)
{
	constexpr float speed = 180;
	angle += speed * elapsedTime;
	//準備完了したらシーン切り替え
	if (IsReady()&& nextScene != nullptr)
	{
		SceneManager::Instance().ChangeScene(nextScene);
		nextScene = nullptr;
	}
}

void SceneLoading::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	//2Dスプライト描画
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float spriteWidth = 256;
		float spriteHeight = 256;
		float positionX = screenWidth - spriteWidth;
		float positionY = screenHeight - spriteHeight;

		sprite->Render(rc,
			positionX, positionY, 0, spriteWidth, spriteHeight,
			angle,
			1, 1, 1, 1);
	}
}

void SceneLoading::DrawGUI()
{
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
	//COM関連の初期化
	CoInitialize(nullptr);
	//次のシーンの初期化
	scene->nextScene->Initialize();
	//スレッドが終わる前に終了か
	CoUninitialize();
	//次のシーンの準備完了設定
	scene->SetReady();
}
