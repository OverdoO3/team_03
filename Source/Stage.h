#pragma once

#include "System//ModelRenderer.h"
#include "System/Sprite.h"
#include <memory>


class Stage
{
public:
	Stage();
	~Stage();

	//çXêV
	void Update(float elapsedTime);

	//ï`âÊ
	void Render(const RenderContext& rc, ModelRenderer* renderer);

private:
	std::unique_ptr<Model>model = nullptr;

	DirectX::XMFLOAT4X4 transform;
public:
	DirectX::XMFLOAT4X4 getTransform() { return transform; }
	Model* getModel() { return model.get(); }
};