#include"SceneTutorial.h"
#include "System/Graphics.h"
#include "System/Input.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneTitle.h"

void SceneTutorial::Initialize()
{
	//スプライト初期化
	spr = std::make_unique<Sprite>("Data/Sprite/0cc60a96ea66ed22.png");
	c = 0;

}

void SceneTutorial::Finalize()
{
}

void SceneTutorial::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButtonDown()& VK_LBUTTON)
	{
		++c;
		
	}
	if (c>=7)
	{
		SceneManager::Instance().ChangeScene(new SceneTitle);
	}
	
	
	


}

void SceneTutorial::Render()
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
		spr->Render(rc, 0, 0, 0, screenWidth, 1080, screenWidth * (c % 5), 1080 * (c / 5), screenWidth, 1080, 0, 1, 1, 1, 1);
	}
}

void SceneTutorial::DrawGUI()
{
}