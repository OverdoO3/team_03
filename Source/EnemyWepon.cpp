#include "EnemyWepon.h"

EnemyWepon::EnemyWepon()
{
	angle = { 0,0,0 };
	UpdateTransfomEuler();
}

EnemyWepon::~EnemyWepon()
{
}

void EnemyWepon::Update(float elapsedTime)
{
	if (timer > 0.0f)
	{
		isAttack = true;
	}
	else
	{
		isAttack = false;
	}
	timer -= elapsedTime * 1;
	UpdateTransfomEuler();
}

void EnemyWepon::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	if (isAttack)
	{
		renderer->RenderCapsule(rc, transform, radius, 1, { 1,0,0,1 });
	}

	if (isCharge)
	{
		renderer->RenderCapsule(rc, transform, radius, 1, { 0,0,1,1 });
	}
}
