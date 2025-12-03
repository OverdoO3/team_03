#pragma once

#include "System/ModelRenderer.h"
#include "System/ShapeRenderer.h"

class ProjectileManager;

class Projectile
{
public:
	Projectile(ProjectileManager* manager);
	virtual ~Projectile() {}

	//更新処理
	virtual void Update(float elapsedTime) = 0;

	//描画
	virtual void Render(const RenderContext& rc, ModelRenderer* renderer) = 0;

	//デバッグ
	virtual void RenderDebugPrimitivbe(const RenderContext& rc, ShapeRenderer* renderer);

	void Destroy();
	
	float GetRadius() const { return radius; }
	//位置
	const DirectX::XMFLOAT3& GetPosition() const { return position; }

	//方向
	const DirectX::XMFLOAT3& GetDirection() const { return direction; }

	//スケール
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

protected:
	void UpdateTransform();

protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 direction = { 0,0,1 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	ProjectileManager* manager = nullptr;
	float radius = 0.5f;
}; 