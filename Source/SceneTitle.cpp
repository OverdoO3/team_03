#include "SceneTitle.h"
#include "System/Graphics.h"
#include "System/Input.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"

void SceneTitle::Initialize()
{
	//スプライト初期化
	sprite = std::make_unique<Sprite>("Data/Sprite/Title.png");
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
		if (mouse.GetOldPositionY() > 400 && mouse.GetOldPositionY() < 500)
		{
			mousec = 1;
		}
		if (mouse.GetOldPositionY() > 600 && mouse.GetOldPositionY() < 700)
		{
			mousec = 2;
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
			0, 0, 0, screenWidth, screenHeight,
			0,
			1, 1, 1, 1);
		sprUI->Render(rc, 730, 400, 0, 400, 100, 800-800 * (mousec % 2), 0, 800, 200, 0, 1, 1, 1, 1);
		sprUI->Render(rc, 730, 600, 0, 400, 100, 800-800 * ( mousec/2), 200, 800, 200, 0, 1, 1, 1, 1);

	}
}

void SceneTitle::DrawGUI()
{
}
