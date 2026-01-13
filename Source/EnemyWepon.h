#pragma once
#include "d3d11.h"
#include "System/ShapeRenderer.h"
#include "Obj.h"

class EnemyWepon : public Obj
{
public:
	EnemyWepon();
	~EnemyWepon();
private:

	float radius = 1.0f;

	float timer = 0.0f;

	bool isAttack;
	bool isCharge;

public:
	void Update(float elapsedTime);
	//”¼ŒaŽæ“¾
	float GetRadius() const { return radius; }

	bool GetIsAttack() const { return isAttack; }

	void SetIsAttack(bool a) { isAttack = a; }
	void SetIsCharge(bool a) { isCharge = a; }

	void SetTimer(float setTimer) { timer = setTimer; }

	void SetRadius(float rad) { radius = rad; }

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)  override;
};