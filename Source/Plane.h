#pragma once
#include "Obj.h"

class Plane : public Obj
{
private:
	bool active;
	float length;
	int type;

	std::unique_ptr<Model> axeTypeModel;
public:
	Plane();
	~Plane() {};
	void Update(float elapsedTime,DirectX::XMFLOAT3 pos,float charge,float angle);
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void SetType(int t) { type = t; }
};