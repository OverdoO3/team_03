#include "Dir.h"
#include "Player.h"
Dir::Dir()
{
	model = std::make_unique<Model>("Data/Model/Player/yazirushi.mdl");
	scale = { 0.05f,0.05f,0.05f };
	UpdateTransfomEuler();
}

Dir::~Dir()
{
}

void Dir::Update(DirectX::XMFLOAT3 pPos)
{
	DirectX::XMFLOAT3 offset = DirectXCommon::GetForward(Player::Instance().GetTransform());
	offset = offset * 0.5f;
	angle.y = atan2f(offset.x, offset.z);
	offset = pPos + offset;
	position.x = offset.x;
	position.y = offset.y;
	position.z = offset.z;
	UpdateTransfomEuler();
}
