#pragma once
#include "Obj.h"
#include "DirectXCommon.h"

class Dir : public Obj
{
public:
	Dir();
	~Dir();

	void Update(DirectX::XMFLOAT3 pPos);
private:

};