#include "Character.h"
#include <algorithm>

void Character::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y,scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}

void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	velocity.x = impulse.x;
	velocity.y = impulse.y;
	velocity.z = impulse.z;
}

void Character::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	renderer->RenderCylinder(rc, position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

bool Character::ApplyDamage(int damage,float invincibleTime)
{
	//ダメージが0じゃないかどうか
	if (damage == 0)return false;
	//生きてないか
	if (health <= 0)return false;
	//無敵時間中はダメージ受けない
	if (this->invincibleTime > 0.01f) return false;

	this->invincibleTime = invincibleTime;

	health -= damage;
	{
		OnDamage();
	}
	return true;
}

void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	DirectX::XMFLOAT3 vec{ vx, 0.0f, vz };
	if (vec.x == 0 && vec.z == 0) return;

	float length = sqrtf(vec.x * vec.x + vec.z * vec.z);
	vec.x /= length;
	vec.z /= length;

	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	float cross = (frontZ * vec.x) - (frontX * vec.z);
	float dot = (vec.x * frontX) + (vec.z * frontZ);

	dot = std::clamp(dot, -1.0f, 1.0f);

	float rot = 1.0f - dot;
	float turn = speed * rot;

	if (cross < 0.0f)
	{
		angle.y -= turn;
	}
	else
	{
		angle.y += turn;
	}
}


void Character::Jump(float speed)
{
	velocity.y = speed;
}

void Character::UpdateVelocity(float elapsedTime)
{
	UpdateVerticalVelocity(elapsedTime);

	UpdateHorizontalVelocity(elapsedTime);

	UpdateVerticalMove(elapsedTime);

	UpdateHorizontalMove(elapsedTime);
	//重力
	//velocity.y += gravity * elapsedTime;
	////移動処理
	//position.y += velocity.y * elapsedTime;
	////地面判定
	//if (position.y < 0.0f)
	//{
	//	position.y = 0.0f;
	//	velocity.y = 0.0f;
	//	//着地した
	//	isGround = true;
	//	if (isGround == true)
	//	{
	//		OnLanding();
	//	}
	//}
	//else
	//{
	//	isGround = false;
	//}
}

void Character::UpdateVerticalVelocity(float elapsedTime)
{
	velocity.y += gravity * elapsedTime;
}

void Character::UpdateVerticalMove(float elapsedTime)
{
	position.y += velocity.y * elapsedTime;

	if (position.y < 2.0f)
	{
		position.y = 2.0f;
		velocity.y = 0.0f;

		//着地した
		isGround = true;
		if (isGround == true)
		{
			OnLanding();
		}
	}
	else
	{
		isGround = false;
	}
}

void Character::UpdateInvincibleTimer(float elapsedTime)
{
	if (invincibleTime > 0.0f)
	{
		invincibleTime -= elapsedTime;
	}
}

bool Character::MoveTowards(const DirectX::XMFLOAT3& target, float speed, float dt, float arriveEps)
{
	// 目標までの差分（XZのみ）
	float dx = target.x - position.x;
	float dz = target.z - position.z;

	float distSq = dx * dx + dz * dz;
	float epsSq = arriveEps * arriveEps;

	// すでに十分近い
	if (distSq <= epsSq)
		return true;

	float dist = std::sqrt(distSq);

	// 進行方向（正規化）
	float nx = dx / dist;
	float nz = dz / dist;

	// 進める距離（このフレームで）
	float step = speed * dt;

	// 行き過ぎ防止（ピッタリ止める）
	if (step >= dist)
	{
		position.x = target.x;
		position.z = target.z;
		return true;
	}

	position.x += nx * step;
	position.z += nz * step;
	return false;
}

void Character::UpdateHorizontalVelocity(float elapsedTime)
{
	float length = sqrtf(velocity.x*velocity.x + velocity.z * velocity.z);
	if (length > 0.0f)
	{
		//摩擦力
		float friction = this->friction * elapsedTime;
		if (!isGround)friction *= airControl;

		//摩擦による横方向の減速処理
		if (length > friction)
		{
			float vx = velocity.x / length;
			float vz = velocity.z / length;
			velocity.x = vx * (length - friction);
			velocity.z = vz * (length - friction);
		}
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}
	//XZ平面の速力加速
	if (length <= maxMoveSpeed)
	{
		//移動ベクトルがゼロベクトル出ないなら加速する
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength > 0.0f)
		{
			float accelaration = this->acceleration * elapsedTime;
			if (!isGround)accelaration *= airControl;
			//移動ベクトルによる加速処理
			float vx = moveVecX / moveVecLength;
			float vz = moveVecZ / moveVecLength;
			velocity.x += vx * accelaration;
			velocity.z += vz * accelaration;
			//最大速度制限
			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > maxMoveSpeed)
			{
				velocity.x = (velocity.x / length) * maxMoveSpeed;
				velocity.z = (velocity.z / length) * maxMoveSpeed;	
			}
		}
	}
	//移動ベクトルをリセット
	moveVecX = 0.0f;
	moveVecZ = 0.0f;
}

void Character::UpdateHorizontalMove(float elapsedTime)
{
	//移動処理
	position.x += velocity.x*elapsedTime;
	position.z += velocity.z*elapsedTime;
}

void Character::Move(float elapsedTime, float vx, float vz, float speed)
{
	//移動方向ベクトルを設定
	moveVecX = vx;
	moveVecZ = vz;

	//最大速度設定
	maxMoveSpeed = speed;
}


