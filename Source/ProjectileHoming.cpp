#include "ProjectileHoming.h"

ProjectileHoming::ProjectileHoming(ProjectileManager* manager) : Projectile(manager)
{
	model = new Model("Data/Model/Sword/Sword.mdl");

	scale.x = scale.y = scale.z = 3.0f;
}

ProjectileHoming::~ProjectileHoming()
{
	delete model;
}

void ProjectileHoming::Update(float elapsedTime)
{
	lifeTimer -= 2 * elapsedTime;
	if (lifeTimer < 0)
	{
		Destroy();
	}
	//移動
	{
		float moveSpeed = this->moveSpeed * elapsedTime;
		position.x += direction.x * moveSpeed;
		position.z += direction.z * moveSpeed;
	}

	//旋回
	{
		float turnSpeed = this->turnSpeed * elapsedTime;
		//ターゲットまでのベクトル
		DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
		DirectX::XMVECTOR Target   = DirectX::XMLoadFloat3(&target);
		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);

		//ゼロベクトル出ないなら回転処理
		DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
		float lengthSq;
		DirectX::XMStoreFloat(&lengthSq, LengthSq);
		if (lengthSq > 0.00001f)
		{
			//単位ベクトル化v
			Vec = DirectX::XMVector3Normalize(Vec);

			//向いている方向ベクトルを算出
			DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);

			//前方向ベクトルとターゲットまでのベクトルの内積を算出
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Direction, Vec);

			float dot;
			DirectX::XMStoreFloat(&dot, Dot);
			dot = max(-1.0f, min(1.0f, dot));
			float angle = acosf(dot);
			
			//二つの単位ベクトルの角度が小さいほど
			//1.0に近づくという性質を利用して回転速度を調整する
			float t = min(angle, turnSpeed);

			if (fabsf(t) > 0.0001f)
			{
				//回転軸を算出
				DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);
				Axis = DirectX::XMVector3Normalize(Axis);

				//回転軸と回転量から回転行列を算出
				DirectX::XMVECTOR Rotation = DirectX::XMQuaternionRotationAxis(Axis, t);

				//現在の前方向を回転させる
				Direction = DirectX::XMVector3Rotate(Direction,Rotation);

				DirectX::XMStoreFloat3(&direction, Direction);
			}
		} 
	}
	UpdateTransform();

	model->UpdateTransform();
}

void ProjectileHoming::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);

	this->direction = direction;
	this->position = position;
	this->target = target;
}

void ProjectileHoming::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target)
{
	this->direction = direction;
	this->position = position;
	this->target = target;
	
	UpdateTransform();
}
