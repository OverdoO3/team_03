#include "WaveManager.h"
#include "EnemyManager.h"
void WaveManager::Initialize()
{
	timer = 0;
	currentWave = 0;
	finishCounts = 0;
	state = WaveState::Waiting;
	timer = 0.0f;
	spawnIndex = 0;
	spawnedCount = 0;
	startDelay = 0;
}

void WaveManager::Update(float elapsedTime,Stage* stage,Pathfinding* path)
{
	if ((int)points.size() == 0)return;
	switch (state)
	{
	case WaveManager::WaveState::Waiting:
		timer += elapsedTime;
		if (timer >= startDelay)
		{
			timer = 0;
			state = WaveState::Spawning;
		}
		break;
	case WaveManager::WaveState::Spawning:
		timer += elapsedTime;
		for (auto& po : points)
		{
			po->Update(elapsedTime,stage,path);
			if (po->isFinish())
			{
				timer = 0;
				state = WaveState::InWave;
			}
		}
		break;
	case WaveManager::WaveState::InWave:
		if (EnemyManager::Instance().GetEnemyCount() == 0)
		{
			state = WaveState::Completed;
		}
		break;
	case WaveManager::WaveState::Completed:
		currentWave++;
		state = WaveState::Waiting;
		break;
	}
}

bool WaveManager::IsWaveCompleted() const
{
	return (EnemyManager::Instance().GetEnemyCount() == 0);
}

void WaveManager::MakeSpawnPoint(DirectX::XMFLOAT3 pos,int spawnType)
{
	std::unique_ptr<SpawnPoint> sp = std::make_unique<SpawnPoint>(spawns[spawnType]);
	sp->SetPosition(pos);
	points.push_back(std::move(sp));
}

void WaveManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	for (auto& po : points)
	{
		po->RenderDebugPrimitive(rc, renderer);
	}
}

void WaveManager::DebugGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 200), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Wave", nullptr, ImGuiWindowFlags_None))
	{
		ImGui::DragInt("WaveNum", &currentWave);
		int a = EnemyManager::Instance().GetEnemyCount();
		ImGui::DragInt("count", &a);
	}
	ImGui::End();
}
