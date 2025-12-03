#include "System/Graphics.h"
#include "Effect.h"
#include "EffectManager.h"

Effect::Effect(const char* filename)
{
	std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());

	//ƒŠƒ\[ƒX‚ð“Ç‚Ýž‚Þ
	char16_t utf16Filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);

	//Effekseer::Manger‚ðŽæ“¾
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerEffect = Effekseer::Effect::Create(effekseerManager, (EFK_CHAR*)utf16Filename);
}

Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& position, float scale)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, position.x, position.y, position.z);
	effekseerManager->SetScale(handle, scale, scale, scale);
	return handle;
}

void Effect::Stop(Effekseer::Handle handle)
{
	Effekseer::ManagerRef effekseerManger = EffectManager::Instance().GetEffekseerManager();

	effekseerManger->StopEffect(handle);
}

void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetLocation(handle, position.x, position.y, position.z);
}

void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}


