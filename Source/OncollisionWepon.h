#pragma once
#include "d3d11.h"
#include "System/ShapeRenderer.h"


class OnCollisionWepon
{
public:
	OnCollisionWepon();
	~OnCollisionWepon();
private:
	DirectX::XMFLOAT3   position = { 0,0,0 };
	DirectX::XMFLOAT3   angle = { 0,0,0 };
	DirectX::XMFLOAT3   scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	float radius = 1.0f;

	float timer = 0.0f;

	bool isAttack;
public:
	void Update(float elapsedTime);

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);
	//更新
	void UpdateTransform();
	//位置取得
	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	//位置設定
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	//スケール取得
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	//スケール設定
	void setScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	//半径取得
	float GetRadius() const { return radius; }

	bool GetIsAttack() const { return isAttack; }

	void SetIsAttack(bool a) { isAttack = a; }

	void SetTimer(float setTimer) { timer = setTimer; }
};