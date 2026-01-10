#include "Stage.h"
#include "nlohmann/json.hpp"
#include "fstream"
using json = nlohmann::json;

//コンストラクタ
Stage::Stage()
{
	//ステージのモデルを読み込む
	model = std::make_unique<Model>("Data/Model/Stage/demostage.mdl");

	scale = { 0.03f,0.03f,0.03f };
	angle = { 0,1.57f,0 };
	UpdateTransfomEuler();

	tower = std::make_unique<Tower>();
	int index = 0;
	std::ifstream file("map.json");
	if (file)
	{
		json data = json::parse(file);
		for (int i = 0;i < WIDTH;i++)
		{
			for (int j = 0;j < HEIGHT;j++)
			{
				map[i][j] = data["cells"][index++].get<int>();
			}
		}
	}
}

Stage::~Stage()
{
}

//更新処理
void Stage::Update(float elapsedTime)
{

}

//描画処理
void Stage::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	//レンダラに描画させる
	renderer->Render(rc, transform, model.get(), ShaderId::Lambert);
}

void Stage::DebugDrawGrid(const RenderContext& rc,ShapeRenderer* renderer,ModelRenderer* modelRenderer)
{
	constexpr float boxHeight = 0.1f;
	constexpr float margin = 0.05f; // 壁とセルの隙間

	for (int z = 0; z < HEIGHT; ++z)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			DirectX::XMFLOAT3 pos = GridToWorld(x, z);
			pos.y += boxHeight / 2.0f; // 床に埋まらないように少し浮かせる

			DirectX::XMFLOAT4 color;
			if (map[z][x] == 0)
			{// 通行可能セル
				color = { 0.0f, 1.0f, 0.0f, 0.5f };
				renderer->RenderBox(
					rc,
					pos,
					{ 0,0,0 },
					{ 1.0f - margin, boxHeight, 1.0f - margin },
					color
				);
			}
			else if (map[z][x] == 1)
			{// 壁セル
				color = { 1.0f, 0.0f, 0.0f, 0.5f };
				pos.y += 0.2f;
				renderer->RenderBox(
					rc,
					pos,
					{ 0,0,0 },
					{ 1.0f - margin, boxHeight, 1.0f - margin },
					color
				);
			}
			else if (map[z][x] == 2)
			{// タワーセル
				color = { 0.0f, 0.0f, 1.0f, 1.0f };
				tower->SetPosition(pos);
				tower->UpdateTransfomEuler();
				tower->Render(rc, modelRenderer);
			}
		}
	}
}


bool Stage::IsWalkable(int x, int z) const
{
	if (x < 0 || z < 0 || x >= WIDTH || z >= HEIGHT)
		return false;

	return map[z][x] == 0;
}

bool Stage::WorldToGrid(const DirectX::XMFLOAT3& pos, int& x, int& z) const
{
	constexpr int OFFSET_X = WIDTH / 2;
	constexpr int OFFSET_Z = HEIGHT / 2;

	x = static_cast<int>(std::floor(pos.x / CELL_SIZE)) + OFFSET_X;
	z = static_cast<int>(std::floor(pos.z / CELL_SIZE)) + OFFSET_Z;

	return (x >= 0 && z >= 0 && x < WIDTH && z < HEIGHT);
}


DirectX::XMFLOAT3 Stage::GridToWorld(int x, int z) const
{
	constexpr int OFFSET_X = WIDTH / 2;
	constexpr int OFFSET_Z = HEIGHT / 2;

	return {
		(x - OFFSET_X + 0.5f) * CELL_SIZE,
		0.0f,
		(z - OFFSET_Z + 0.5f) * CELL_SIZE
	};
}

void Stage::DrawDebugGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 200), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Tower", nullptr, ImGuiWindowFlags_None))
	{
		int a = tower->GetHP();
		ImGui::DragInt("HP", &a);
	}
	ImGui::End();
}


