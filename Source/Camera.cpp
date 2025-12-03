#include "Camera.h"

void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	DirectX::XMVECTOR Eye   = DirectX::XMLoadFloat3(&eye);
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	DirectX::XMVECTOR Up    = DirectX::XMLoadFloat3(&up);
	DirectX::XMMATRIX View  = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View);

	//カメラの方向を取り出す
	this->right.x = view._11;
	this->right.y = view._21;
	this->right.z = view._31;

	this->up.x = view._12;
	this->up.y = view._22;
	this->up.z = view._32;

	this->front.x = view._13;
	this->front.y = view._23;
	this->front.z = view._33;

	//視点、注視点を保存
	this->eye = eye;
	this->focus = focus;
}

void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);

	DirectX::XMStoreFloat4x4(&projection, Projection);
}