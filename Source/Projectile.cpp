#include "Projectile.h"
#include "ProjectileManager.h"

Projectile::Projectile(ProjectileManager* manager) :manager(manager)
{
	manager->Register(this);
}

void Projectile::RenderDebugPrimitivbe(const RenderContext& rc, ShapeRenderer* renderer)
{
	renderer->RenderSphere(rc, position, radius,DirectX::XMFLOAT4(0,0,0,1));
}

void Projectile::Destroy()
{
	manager->Remove(this);
}

//行列更新
void Projectile::UpdateTransform()
{
	//とりあえず
	//transform._11 = 1.0f * scale.x;
	//transform._12 = 0.0f * scale.x;
	//transform._13 = 0.0f * scale.x;
	//transform._14 = 0.0f;
	//transform._21 = 0.0f * scale.y;
	//transform._22 = 1.0f * scale.y;
	//transform._23 = 0.0f * scale.y;
	//transform._24 = 0.0f;
	//transform._31 = 0.0f * scale.z;
	//transform._32 = 0.0f * scale.z;
	//transform._33 = 1.0f * scale.z;
	//transform._34 = 0.0f;
	//transform._41 = position.x;
	//transform._42 = position.y;
	//transform._43 = position.z;
	//transform._44 = 1.0f;

	DirectX::XMVECTOR Front, Up, Right;

	//前ベクトルを算出
	Front = DirectX::XMLoadFloat3(&direction);
	Front = DirectX::XMVector3Normalize(Front);

	//仮の上ベクトルを算出
	Up = DirectX::XMVectorSet(0.001f, 1, 0, 0);
	Up = DirectX::XMVector3Normalize(Up);

	//右ベクトルを算出
	Right = DirectX::XMVector3Cross(Up, Front);
	Right = DirectX::XMVector3Normalize(Right);

	//上方向ベクトルを算出
	Up = DirectX::XMVector3Cross(Front, Right);
	Up = DirectX::XMVector3Normalize(Up);

	//計算結果を取り出し
	DirectX::XMFLOAT3 right, up, front;
	DirectX::XMStoreFloat3(&right, Right);
	DirectX::XMStoreFloat3(&up, Up);
	DirectX::XMStoreFloat3(&front, Front);

	//ベクトルを行列に
	transform._11 = right.x;
	transform._12 = right.y;
	transform._13 = right.z;
	transform._14 = 0.0f;
	transform._21 = up.x;
	transform._22 = up.y;
	transform._23 = up.z;
	transform._24 = 0.0f;
	transform._31 = front.x;
	transform._32 = front.y;
	transform._33 = front.z;
	transform._34 = 0.0f;
	transform._41 = position.x;
	transform._42 = position.y;
	transform._43 = position.z;
	transform._44 = 1.0f;

	this->direction = front;
}
