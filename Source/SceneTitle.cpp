#include "SceneTitle.h"
#include "System/Graphics.h"
#include "System/Input.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include"SceneTutorial.h"


#include "Player.h"

void SceneTitle::Initialize()
{
	//スプライト初期化
	sprite = std::make_unique<Sprite>("Data/Sprite/title_font_UI.png");
	sprUI = std::make_unique<Sprite>("Data/Sprite/title_UI.png");

	Player::Instance().Initialize();
	tower = std::make_unique<Tower>();

	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),//視点
		DirectX::XMFLOAT3(0, 0, 0),   //注視店
		DirectX::XMFLOAT3(0, 1, 0)    //上方向
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),//視野角
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),//アス比
		0.1f,//クリップ近
		1000.0f//クリップ遠
	);
	cameraController = std::make_unique<CameraController>();

	tower->SetPosition({ 10,10,10 });
	Player::Instance().SetPosition({ 10,10,10 });
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
	tower->Update(elapsedTime);
}

void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();

	//描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	rc.view = Camera::Instance().GetView();
	rc.projection = Camera::Instance().GetProjection();


	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
	DirectX::XMMATRIX VP = V * P;
	DirectX::XMFLOAT4X4 vp;
	DirectX::XMStoreFloat4x4(&vp, VP);
	
	{
		//tower->Render(rc, modelRenderer);
		Player::Instance().Render(rc, modelRenderer);
	}

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
