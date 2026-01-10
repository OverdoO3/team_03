#include "Wepon.h"

Wepon::Wepon()
{
	angle = { 0,0,0 };
	UpdateTransfomEuler();
}

Wepon::~Wepon()
{
}

void Wepon::Update(float elapsedTime)
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

void Wepon::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	if (isAttack)
	{
		renderer->RenderCapsule(rc, transform, radius, 1, { 1,0,0,1 });
	}
	else
	{
		renderer->RenderCapsule(rc, transform, radius, 1, { 1,1,1,1 });
	}
}
