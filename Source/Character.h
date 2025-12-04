#pragma once

#include<DirectXMath.h>
#include"System/ShapeRenderer.h"
#include"System/Sprite.h"
#include"System/Model.h"
#include<memory>

class Character
{
public:
	Character(){}
	virtual ~Character(){}
	//行列更新処理
	void UpdateTransform();
	//衝撃を加える
	void AddImpulse(const DirectX::XMFLOAT3& impulse);
	//位置取得
	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	//位置設定
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	//回転点取得
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }
	//回転設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
	//スケール取得
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	//スケール設定
	void setScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	//半径取得
	float GetRadius() const { return radius; }
	//デバッグ描画
	virtual void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);
	//接地判定
	bool IsGround() const { return isGround; }
	//高さ
	float GetHeight() { return height; }
	//ダメージを与える
	bool ApplyDamage(int damage, float invincibleTime);

	int GetHP() { return health; }
protected:
	DirectX::XMFLOAT3   position = { 0,0,0 };
	DirectX::XMFLOAT3   angle = { 0,0,0 };
	DirectX::XMFLOAT3   scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	float radius = 0.5f;
	float gravity = -30.0f;
	float height = 2.0f;
	int health = 5;
	float invincibleTime = 1.0f;
	float friction = 15.0f;
	float acceleration = 50.0f;
	float maxMoveSpeed = 5.0f;
	float moveVecX = 0.0f;
	float moveVecZ = 0.0f;
	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	bool isGround = false;
	float airControl = 0.3f;

	std::unique_ptr<Model> model = nullptr;

	void Move(float elapsedTime, float vx, float vz, float speed);

	void Turn(float elapsedTime, float vx, float vz, float speed);

	void Jump(float speed);

	void UpdateVelocity(float elapsedTime);

	void UpdateVerticalVelocity(float elapsedTime);

	void UpdateVerticalMove(float elapsedTime);

	virtual void OnLanding() {};

	virtual void OnDamage() {}

	virtual void OnDead() {}

	void UpdateInvincibleTimer(float elapsedTime);
private:
	void UpdateHorizontalVelocity(float elapsedTime);

	void UpdateHorizontalMove(float elapsedTime);
};