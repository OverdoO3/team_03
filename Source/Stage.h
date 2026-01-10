#pragma once

#include "System//ModelRenderer.h"
#include "System/Sprite.h"
#include <memory>
#include "Obj.h"
#include "tower.h"

class Stage : public Obj
{
public:
	Stage();
	~Stage();

	static constexpr int WIDTH = 38;
	static constexpr int HEIGHT = 38;
	static constexpr float CELL_SIZE = 2.0f;

	//çXêV
	void Update(float elapsedTime);

	//ï`âÊ
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void DebugDrawGrid(const RenderContext& rc,ShapeRenderer* renderer,ModelRenderer* ModelRenderer);

	bool IsWalkable(int x, int z) const;

	bool WorldToGrid(const DirectX::XMFLOAT3& pos, int& x, int& z) const;
	DirectX::XMFLOAT3 GridToWorld(int x, int z) const;

	Tower* GetTower() { return tower.get(); }

	void DrawDebugGUI() override;
private:
	std::unique_ptr<Model>model = nullptr;
	std::unique_ptr<Tower>tower = nullptr;

	int map[WIDTH][HEIGHT];
private:
	
public:
	Model* getModel() { return model.get(); }
};