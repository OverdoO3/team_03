#include "Stage.h"
#include "nlohmann/json.hpp"
#include "fstream"
using json = nlohmann::json;

//コンストラクタ
Stage::Stage()
{
	//ステージのモデルを読み込む
	model = std::make_unique<Model>("Data/Model/Stage/field.mdl");

	nexus = std::make_unique<Nexus>();

	position = { 0.0f,-2.8f,0.0f};
	scale = { 0.02f,0.02f,0.02f };
	angle = { 0,1.57f,0 };
	UpdateTransfomEuler();

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
				if (map[i][j] == 2)
				{
					DirectX::XMFLOAT3 pos = GridToWorld(j, i);
					auto t = std::make_unique<Tower>();
					t->SetPosition(pos);
					t->UpdateTransfomEuler();
					towers.emplace_back(std::move(t));
				}
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
	for (auto& t : towers)
	{
		t->Update(elapsedTime);
	}
}

//描画処理
void Stage::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	//レンダラに描画させる
	renderer->Render(rc, transform, model.get(), ShaderId::Lambert);
	for (auto& t : towers)
	{
		t->Render(rc, renderer);
	}
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
			pos.y -= 2.0f;

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
				renderer->RenderBox(
					rc,
					pos,
					{ 0,0,0 },
					{ 1.0f - margin, 2.0f, 1.0f - margin },
					color
				);
			}
			else if (map[z][x] == 3)
			{// スポーンセル
				color = { 1.0f, 0.0f, 1.0f, 1.0f };
				renderer->RenderBox(
					rc,
					pos,
					{ 0,0,0 },
					{ 1.0f - margin, boxHeight, 1.0f - margin },
					color
				);
			}
			else if (map[z][x] == 4)
			{// NEXUSセル
				color = { 0.0f, 1.0f, 1.0f, 1.0f };
				renderer->RenderBox(
					rc,
					pos,
					{ 0,0,0 },
					{ 1.0f - margin, 100, 1.0f - margin },
					color
				);
			}
		}
	}
}


bool Stage::IsWalkable(int x, int z) const
{
	if (x < 0 || z < 0 || x >= WIDTH || z >= HEIGHT)
		return false;

	return (map[z][x] == 0||map[z][x] == 2||map[z][x] == 3);
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
		(x - OFFSET_X + 1.0f) * CELL_SIZE,
		0.0f,
		(z - OFFSET_Z) * CELL_SIZE
	};
}

void Stage::DrawDebugGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 1000, pos.y + 200), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Tower", nullptr, ImGuiWindowFlags_None))
	{
		int a = nexus->GetHP();
		ImGui::DragInt("HP", &a);
	}
	ImGui::End();
}


