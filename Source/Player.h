#pragma once

#include "System/ModelRenderer.h"
#include "ProjectileManager.h"
#include "Character.h"
#include "System/AudioSource.h"
#include "Stage.h"
#include "Wepon.h"

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

	bool InputMove(float elapsedTime, const int(&maps)[38][38]);

	void Update(float elapsedTime, const int(&maps)[38][38]);

	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	void CollisionWeponVsEnemies();

	void InputAttack();
	void InputRush(float elapsedTime, const int(&maps)[38][38]);

	void setStage(Stage* s) { stage = s; }

	void PlayAnimation(int index, bool loop);
	void PlayAnimation(const char* name, bool loop);

	void UpdateAnimation(float elapsedTime);

	void MoveWithCollision(float elapsedTime,float dx,float dz,const int(&maps)[38][38],bool isRush = false);

	void ChangeWepon();
private:
	//スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720.0f);
	void CollisionPlyerVsEnemies();

	std::unique_ptr<Effect> trailEffect = nullptr;
	std::unique_ptr<Effect> WeponTrailEffect = nullptr;
	Effekseer::Handle trailHandle = -1;

	std::unique_ptr<AudioSource> hitSE = nullptr;
	std::unique_ptr<Wepon> col = nullptr;
	std::shared_ptr<Model> wepons[3];
	int riskGauge[3] = {0,0,0};

	int maxGauge = 100;

	Stage* stage;
	bool wasPressed = false;
	bool wasPressedR = false;

	int									animationIndex = -1;
	float								animationSeconds = 0.0f;
	bool								animationLoop = false;
	bool								animationPlaying = false;
	float								animationBlendSecondsLength = 0.2f;

	float rushSpeed = 14.0f;
	DirectX::XMFLOAT3 rushVec = { 0,0,0 };
	float rushTimer = 0.0f;
	float rushTime = 0.2f;
	float rushDist = 0.0f;
	bool isChargeRush;

	enum class HaveWepon
	{
		Sword,
		Axe,
		Spere,
	};

	HaveWepon nowWepon = HaveWepon::Axe;

	int swordDamage = 5;
	int AxeDamage = 10;
	int SpearDamage = 3;

	DirectX::XMFLOAT3 WeponTipPos{};

	enum class State
	{
		Idle,
		Run,
		Special,
		Attack,
	};
	State								state = State::Idle;
};