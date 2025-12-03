#pragma once

#include <DirectXMath.h>

#define CAMERA_ANGLE_X 30
#define CAMERA_ANGLE_Y 40

class CameraController
{
public:
	void Update(float elapsedTime);

	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

private:
	DirectX::XMFLOAT3 target = { 0,0,0 }; //’‹“_
	DirectX::XMFLOAT3 angle = { 0,0,0 };  //‰ñ“]Šp“x
	float rollSpeed = DirectX::XMConvertToRadians(90); //‰ñ“]‘¬“x
	float range = 30.0f; //‹——£

	float maxAngleX = DirectX::XMConvertToRadians(45);
	float minAngleX = DirectX::XMConvertToRadians(-45);
};