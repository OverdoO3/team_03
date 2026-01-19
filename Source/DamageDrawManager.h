#pragma once
#include "DamageDraw.h"
#include <vector>

class DamegeDrawManager
{
public:
	static DamegeDrawManager& Instance()
	{
		static DamegeDrawManager instance;
		return instance;
	}

	DamegeDrawManager() {};
	~DamegeDrawManager() {};

	void Update(float elapsedTime);
	void Render(RenderContext& rc);

	void makeTexts(int value, DirectX::XMFLOAT2 pos, float size, DirectX::XMFLOAT4 color, float timer);

private:
	std::vector<std::unique_ptr<DamageDraw>> texts;
};
