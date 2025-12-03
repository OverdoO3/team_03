#include "ProjectileStraight.h"

ProjectileStraight::ProjectileStraight(ProjectileManager* manager) : Projectile(manager)
{
	model = new Model("Data/Model/Sword/Sword.mdl");

	//表示サイズを調整
	//scale.x = scale.y = scale.z = 0.5f;
	scale.x = scale.y = scale.z = 10.0f;
}


ProjectileStraight::~ProjectileStraight()
{
	delete model;
}

void ProjectileStraight::Update(float elapsedTime)
{
	lifeTimer -= 2 * elapsedTime;
	if (lifeTimer < 0)
	{
		Destroy();
	}

	float speed = this->speed * elapsedTime;
	position.x += direction.x * speed;
	position.z += direction.z * speed;

	UpdateTransform();

	model->UpdateTransform();
}

void ProjectileStraight::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);
}

void ProjectileStraight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	this->direction = direction;
	this->position = position;
}
	