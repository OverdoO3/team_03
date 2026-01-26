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
	stage = std::make_unique<Stage>();

	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 20, 25),//視点
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
		if (mouse.GetOldPositionY() > 450 && mouse.GetOldPositionY() < 550)
		{
			mousec = 1;
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
			}
		}
		if (mouse.GetOldPositionY() > 600 && mouse.GetOldPositionY() < 700)
		{
			mousec = 2;
		}
		if (mouse.GetOldPositionY() > 750 && mouse.GetOldPositionY() < 850)
		{
			mousec = 4;
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				SceneManager::Instance().ChangeScene(new SceneTutorial);
			}
		}
		if (mouse.GetOldPositionY() > 900 && mouse.GetOldPositionY() < 1000)
		{
			mousec = 8;
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				PostQuitMessage(0);
			}
		}
	}

		Camera::Instance().SetLookAt(
		camera_eye,//視点
		camera_target,   //注視店
		DirectX::XMFLOAT3(0, 1, 0)    //上方向
	);

	Player::Instance().setScale(sca);
	Player::Instance().SetPosition(pos);
	Player::Instance().SetAngle(ang);
	Player::Instance().UpdateTransform();
	Player::Instance().GetDirObj()->SetPosition({100,100,100});
	Player::Instance().GetDirObj()->UpdateTransfomEuler();

	stage->Update(elapsedTime);
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
		Player::Instance().GetDirObj()->SetPosition({ 100,100,100 });
		Player::Instance().Render(rc, modelRenderer);

		stage->Render(rc, modelRenderer);
	}

	//2Dスプライト描画
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		sprite->Render(rc,
			530, 100, 0, 800, 300,
			0,
			1, 1, 1, 1);
		sprUI->Render(rc, 730, 450, 0, 400, 100, 800-800 * (mousec % 2), 0, 800, 200, 0, 1, 1, 1, 1);
		sprUI->Render(rc, 730, 600, 0, 400, 100, 800-800 * ( mousec/2%2), 200, 800, 200, 0, 1, 1, 1, 1);
		sprUI->Render(rc, 730, 750, 0, 400, 100, 800-800 * ( mousec/4%2), 400, 800, 200, 0, 1, 1, 1, 1);
		sprUI->Render(rc, 730, 900, 0, 400, 100, 800-800 * ( mousec/8),  600, 800, 200, 0, 1, 1, 1, 1);

	}
}

void SceneTitle::DrawGUI()
{
	/*if(ImGui::Begin("tran",nullptr,ImGuiBackendFlags_None))
	{
		ImGui::DragFloat3("pos", &pos.x);
		ImGui::DragFloat3("ang", &ang.x);
		ImGui::DragFloat3("sca", &sca.x);

		ImGui::DragFloat3("camerapos", &camera_eye.x);
		ImGui::DragFloat3("cameratarget", &camera_target.x);
	}
	ImGui::End();*/
}
