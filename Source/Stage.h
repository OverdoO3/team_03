#pragma once

#include "System//ModelRenderer.h"
#include "System/Sprite.h"
#include <memory>
#include "Obj.h"

class Stage : public Obj
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

public:
	Model* getModel() { return model.get(); }
};