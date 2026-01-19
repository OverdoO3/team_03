#include "DamageDrawManager.h"

void DamegeDrawManager::Update(float elapsedTime)
{
	for (auto& a : texts)
	{
		a->Update(elapsedTime);
	}
}

void DamegeDrawManager::Render(RenderContext& rc)
{
	for (auto& a : texts)
	{
		a->Render(rc);
	}
}

void DamegeDrawManager::makeTexts(int value, DirectX::XMFLOAT2 pos, float size, DirectX::XMFLOAT4 color, float timer)
{
	DamageDraw *a = new DamageDraw( value, pos, size, color, timer);
	texts.emplace_back(a);
}


