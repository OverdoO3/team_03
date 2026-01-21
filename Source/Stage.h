#pragma once

#include "System//ModelRenderer.h"
#include "System/Sprite.h"
#include <memory>
#include "Obj.h"
#include "tower.h"
#include "nexus.h"

class Stage : public Obj
{
public:
	Stage();
	~Stage();

	static constexpr int WIDTH = 60;
	static constexpr int HEIGHT = 60;
	static constexpr float CELL_SIZE = 2.0f;

	//çXêV
	void Update(float elapsedTime);

	//ï`âÊ
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void DebugDrawGrid(const RenderContext& rc,ShapeRenderer* renderer,ModelRenderer* ModelRenderer);

	bool IsWalkable(int x, int z) const;

	bool WorldToGrid(const DirectX::XMFLOAT3& pos, int& x, int& z) const;
	DirectX::XMFLOAT3 GridToWorld(int x, int z) const;

	Tower* GetTower(int num) { return towers[num].get(); }
	std::vector<std::unique_ptr<Tower>>& GetTower() { return towers; }

	std::unique_ptr<Nexus>& GetNexus() { return nexus; }

	void DrawDebugGUI() override;
private:
	std::unique_ptr<Model>model = nullptr;
	std::vector<std::unique_ptr<Tower>> towers;

	std::unique_ptr<Nexus>nexus = nullptr;

	int map[WIDTH][HEIGHT];
private:
	
public:
	Model* getModel() { return model.get(); }
};