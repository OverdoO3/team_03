#pragma once

#include<DirectXMath.h>

class Camera
{
private:
	Camera(){}
	~Camera(){}

public:
	//インスタンス
	static Camera& Instance()
	{
		static Camera camera;
		return camera;
	}
	//指定方向向く
	void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);
	//描画範囲設定
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);
	//びゅー行列取得
	const DirectX::XMFLOAT4X4& GetView() const { return view; }
	//プロジェクション行列取得
	const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }
	//視点取得
	const DirectX::XMFLOAT3& GetEye() const { return eye; }
	//注視点取得
	const DirectX::XMFLOAT3& GetForcus() const { return focus; }
	//上方向取得
	const DirectX::XMFLOAT3& GetUp() const { return up; }
	//前方向取得
	const DirectX::XMFLOAT3& GetFront() const { return front; }
	//右方向取得
	const DirectX::XMFLOAT3& GetRight() const { return right; }

private:
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;

	DirectX::XMFLOAT3 eye;
	DirectX::XMFLOAT3 focus;

	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 front;
	DirectX::XMFLOAT3 right;
};