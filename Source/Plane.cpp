#include "Plane.h"

Plane::Plane()
{
	model = std::make_unique<Model>("Data/Model/Player/eff.mdl");
	axeTypeModel = std::make_unique<Model>("Data/Model/Player/effAxe.mdl");
	scale = { 0.01f,0.01f,0.01f };
	UpdateTransfomEuler();
	model->UpdateTransform();
}

void Plane::Update(float elapsedTime, DirectX::XMFLOAT3 pos,float charge,float angleY)
{
	float length = charge * 0.1f;
	if (type == 2)
	{
		scale = { 0.01f,0.01f,0.01f };
		scale.z = length * 0.5f;
	}
	else if(type == 1)
	{
		scale = { 0.01f,0.01f,0.01f };
		scale.x = length * 0.1f;
		scale.z = length * 0.1f;
	}
	
	position = pos;
	position.y += 0.1f;

	position.x += sinf(angleY) * (scale.z* 0.5f);
	position.z += cosf(angleY) * (scale.z * 0.5f);

	this->angle.y = angleY;
	UpdateTransfomEuler();
	model->UpdateTransform();
}

void Plane::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	if (type == 2)
	{
		renderer->Render(rc, transform, model.get(), ShaderId::Lambert);
	}
	else if(type == 1)
	{
		renderer->Render(rc, transform, axeTypeModel.get(), ShaderId::Lambert);
	}
}
