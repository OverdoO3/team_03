#include "OncollisionWepon.h"

OnCollisionWepon::OnCollisionWepon()
{
}

OnCollisionWepon::~OnCollisionWepon()
{
}

void OnCollisionWepon::Update(float elapsedTime)
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
	UpdateTransform();
}

void OnCollisionWepon::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	if (isAttack)
	{
		renderer->RenderSphere(rc, position, radius, { 1,0,0,1 });
	}
	else
	{
		renderer->RenderSphere(rc, position, radius, { 1,1,1,1 });
	}
}

void OnCollisionWepon::UpdateTransform()
{
}
