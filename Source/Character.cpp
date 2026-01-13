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

// アニメーション再生
void Character::PlayAnimation(int index, bool loop)
{
	animationPlaying = true;
	animationLoop = loop;
	animationIndex = index;
	animationSeconds = 0.0f;
}

void Character::PlayAnimation(const char* name, bool loop)
{
	int index = 0;
	const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
	for (const ModelResource::Animation& animation : animations)
	{
		if (animation.name == name)
		{
			PlayAnimation(index, loop);
			return;
		}
		++index;
	}
}


// アニメーション更新処理
void Character::UpdateAnimation(float elapsedTime)
{
	if (animationPlaying)
	{
		//アニメーション切り替え時のブレンド率を計算
		float blendRate = 1.0f;
		if (animationSeconds < animationBlendSecondsLength)
		{
			blendRate = (animationSeconds / animationBlendSecondsLength);
		}

		std::vector<Model::Node>& nodes = model->GetNodes();

		//アニメーションを取得
		const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
		const ModelResource::Animation& animation = animations.at(animationIndex);

		animationSeconds += elapsedTime;

		if (animationSeconds >= animation.secondsLength)
		{
			if (animationLoop)
			{
				animationSeconds = 0;
			}
			else
			{
				animationPlaying = false;
			}
		}

		const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
		int keyCount = static_cast<int>(keyframes.size());

		for (int keyIndex = 0;keyIndex < keyCount - 1; ++keyIndex)
		{
			//現在の時間がどのキーフレームかを判定する
			const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
			const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
			if (animationSeconds >= keyframe0.seconds && animationSeconds < keyframe1.seconds)
			{
				//再生時間とキーフレームの時間から補完率を算出
				float rate = ((animationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds));

				//すべてのノードの姿勢を計算する
				int nodeCount = static_cast<int>(nodes.size());
				for (int nodeIndex = 0; nodeIndex < nodeCount;++nodeIndex)
				{
					const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
					const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

					//ノード取得
					Model::Node& node = nodes[nodeIndex];

					if (blendRate < 1.0f)
					{
						//現在と次の姿勢を補完
						DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&node.scale);
						DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
						DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&node.rotate);
						DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
						DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&node.translate);
						DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);

						DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, blendRate);
						DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, blendRate);
						DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, blendRate);

						DirectX::XMStoreFloat3(&node.scale, S);
						DirectX::XMStoreFloat4(&node.rotate, R);
						DirectX::XMStoreFloat3(&node.translate, T);
					}
					else
					{
						//前のキーフレームと次のキーフレームの姿勢を補完
						DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scale);
						DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
						DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotate);
						DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
						DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translate);
						DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);

						DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
						DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
						DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);

						DirectX::XMStoreFloat3(&node.scale, S);
						DirectX::XMStoreFloat4(&node.rotate, R);
						DirectX::XMStoreFloat3(&node.translate, T);
					}
				}
			}
		}
	}
	//行列更新
	model->UpdateTransform();
}

