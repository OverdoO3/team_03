#include "DamageDraw.h"

void DamageDraw::Update(float elapsedTime)
{
	timer -= elapsedTime;
}

void DamageDraw::Render(RenderContext& rc)
{	
	scoreRender::ScoreRenderDigit_NoHead_Spacing(rc, Spr.get(), value, 100.0f, 100.0f, pos.x, pos.y + 1.0f / timer, 0.0f, 0.0f, timer * 5.0f,size,0);
}
