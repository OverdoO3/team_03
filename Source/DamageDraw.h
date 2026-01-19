#pragma once
#include "ScoreRender.h"
#include "DirectXMath.h"
#include "System/Graphics.h"

class DamageDraw
{
public:
	DamageDraw(int value, DirectX::XMFLOAT2 pos, float size, DirectX::XMFLOAT4 color, float timer)
	{
		this->value = value;
		this->pos = pos;
		this->size = size;
		this->color = color;
		this->timer = timer;
		Spr = std::make_unique<Sprite>("Data/Sprite/number_UI.png");
	}
	~DamageDraw() {};

	float getTimer() { return timer; }

	void Update(float elapsedTime);
	void Render(RenderContext& rc);
private:
	float timer;
	int value;
	float size;
	DirectX::XMFLOAT2 pos;
	DirectX::XMFLOAT4 color;

	std::unique_ptr<Sprite> Spr = nullptr;
};