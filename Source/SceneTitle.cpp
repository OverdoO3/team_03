#include "SceneTitle.h"
#include "System/Graphics.h"
#include "System/Input.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include"SceneTutorial.h"

void SceneTitle::Initialize()
{
	//スプライト初期化
	sprite = std::make_unique<Sprite>("Data/Sprite/title_font_UI.png");
	sprUI = std::make_unique<Sprite>("Data/Sprite/title_UI.png");
	

}

void SceneTitle::Finalize()
{
}

void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	const GamePadButton anyButton =
		GamePad::BTN_A
		| GamePad::BTN_B
		| GamePad::BTN_X
		| GamePad::BTN_Y
		;
	if (gamePad.GetButtonDown() & anyButton)
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
	}
	Mouse& mouse = Input::Instance().GetMouse();
	mousec = 0;
	if (mouse.GetOldPositionX() > 730 && mouse.GetOldPositionX() < 1130)
	{
		if (mouse.GetOldPositionY() > 500 && mouse.GetOldPositionY() < 600)
		{
			mousec = 1;
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
			}
		}
		if (mouse.GetOldPositionY() > 650 && mouse.GetOldPositionY() < 750)
		{
			mousec = 2;
		}
		if (mouse.GetOldPositionY() > 800 && mouse.GetOldPositionY() < 900)
		{
			mousec = 4;
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				SceneManager::Instance().ChangeScene(new SceneTutorial);
			}
		}
	}
	

}

void SceneTitle::Render()
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
		sprite->Render(rc,
			530, 100, 0, 800, 300,
			0,
			1, 1, 1, 1);
		sprUI->Render(rc, 730, 500, 0, 400, 100, 800-800 * (mousec % 2), 0, 800, 200, 0, 1, 1, 1, 1);
		sprUI->Render(rc, 730, 650, 0, 400, 100, 800-800 * ( mousec%4/2), 200, 800, 200, 0, 1, 1, 1, 1);
		sprUI->Render(rc, 730, 800, 0, 400, 100, 800-800 * ( mousec/4), 400, 800, 200, 0, 1, 1, 1, 1);

	}
}

void SceneTitle::DrawGUI()
{
}
