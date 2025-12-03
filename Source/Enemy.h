#pragma once

#include "System/ModelRenderer.h"
#include "Character.h"

//エネミー
class Enemy : public Character
{
public:
	Enemy() {}
	~Enemy() override{}

	virtual void Update(float elapsedTime) = 0;

	//破棄
	void Destroy();

	virtual void Render(const RenderContext& rc, ModelRenderer* renderer) = 0;

	virtual void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
	{
        // 共通のデバッグ描画（例：当たり判定の円柱）
        renderer->RenderCylinder(rc, GetPosition(), GetRadius(), GetHeight(), DirectX::XMFLOAT4(0, 0, 0, 1));
    }


};