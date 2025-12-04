#pragma once

#include "System/ModelRenderer.h"
#include "ProjectileManager.h"
#include "Character.h"
#include "Effect.h"
#include "System/AudioSource.h"
#include "Stage.h"
#include "OncollisionWepon.h"

//プレイヤー
class Player : public Character
{
private:
	Player() {};
	~Player() override{};
public:
	static Player& Instance()
	{
		static Player instance;
		return instance;
	}
	void Initialize();

	void Finalize();

	void DrawDebugGUI();

	void InputMove(float elapsedTime);

	void Update(float elapsedTime);

	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	void CollisionWeponVsEnemies();

	void InputProjectile();

	void InputJump();

	void setStage(Stage* s) { stage = s; }
protected:
	void OnLanding() override;
private:
	//スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720.0f);
	void CollisionPlyerVsEnemies();
	float jumpSpeed = 12.0f;
	int jumpCount = 0;
	int jumpLimit = 2;
	ProjectileManager projectileManager;
	std::unique_ptr<Effect> hitEffect = nullptr;
	std::unique_ptr<Effect> trailEffect = nullptr;
	Effekseer::Handle trailHandle = -1;
	std::unique_ptr<AudioSource> hitSE = nullptr;

	std::unique_ptr<OnCollisionWepon> col = nullptr;

	Stage* stage;
	bool wasPressed = false;
};