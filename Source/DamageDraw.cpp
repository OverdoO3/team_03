#include "DamageDraw.h"

void DamageDraw::Update(float elapsedTime)
{
	timer -= elapsedTime;
}

void DamageDraw::Render(RenderContext& rc)
{	
	scoreRender::ScoreRenderDigit_NoHead_Spacing(rc, Spr.get(), value, 100.0f, 100.0f, pos.x, pos.y, 0.0f, 0.0f, timer * 5.0f,size,0);
	//Spr->Render(rc, 100, 100, 0, 100, 100, 0, 1, 1, 1, 1);
}
