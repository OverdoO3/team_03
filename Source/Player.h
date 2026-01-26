#pragma once

#include "System/ModelRenderer.h"
#include "ProjectileManager.h"
#include "Character.h"
#include "System/AudioSource.h"
#include "Stage.h"
#include "Wepon.h"
#include "Plane.h"
#include "Dir.h"

//プレイヤー
class Player : public Character
{
private:
	Player() {};
	~Player() override{};

	static constexpr int WIDTH = 60;
	static constexpr int HEIGHT = 60;
public:
	static Player& Instance()
	{
		static Player instance;
		return instance;
	}
	void Initialize();

	void Finalize();

	void DrawDebugGUI();

	bool InputMove(float elapsedTime, const int(&maps)[WIDTH][HEIGHT]);

	void Update(float elapsedTime, const int(&maps)[WIDTH][HEIGHT]);

	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void RenderUI(const RenderContext& rc);

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	void CollisionWeponVsEnemies();

	void InputAttack();
	void InputAvoid();

	void InputRush(float elapsedTime, const int(&maps)[WIDTH][HEIGHT]);
	void InputCharge(float elapsedTime);
	void InputChain(float elapsedTime);

	void setStage(Stage* s) { stage = s; }

	void MoveWithCollision(float elapsedTime,float dx,float dz,const int(&maps)[WIDTH][HEIGHT],float value = 1);

	void ChangeWepon();

	void UpdateWeponCollisionFromMotion();

	float GetHitStopTimer() {return hitStopTimer;}

	Dir* GetDirObj() { return dirModel.get(); }

	bool GetIsDead() { return isDead; }

	float GetRespawnTimer() { return respawnTimer;}

	float GetNowRiskGauge() { return riskGauge[(int)nowWepon]; }
private:
	bool isDead = false;
	float respawnTimer = 0;

	float tim;
	//スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720.0f);
	void CollisionPlyerVsEnemies();

	std::unique_ptr<Effect> WeponTrailEffect = nullptr;
	Effekseer::Handle trailHandle = -1;

	std::unique_ptr<Effect> riskAura = nullptr;
	Effekseer::Handle riskAuraHandle = -1;

	std::unique_ptr<Effect> SpearDash = nullptr;
	Effekseer::Handle SpearDashHandle = -1;

	std::unique_ptr<Effect> SwordSlash = nullptr;
	Effekseer::Handle SwordSlashHandle = -1;

	std::unique_ptr<Plane>plane = nullptr;

	float testAngle = 0;

	std::unique_ptr<AudioSource> hitSE = nullptr;
	std::unique_ptr<Wepon> weponCol = nullptr;
	std::shared_ptr<Model> wepons[3];

	std::unique_ptr<Dir> dirModel = nullptr;
	
	int riskGauge[3] = {0,0,0};

	int maxGauge = 100;

	std::unique_ptr<Sprite> sprHP = nullptr;
	std::unique_ptr<Sprite> sprtower= nullptr;
	std::unique_ptr<Sprite> sprRisuku = nullptr;
	std::unique_ptr<Sprite> sprwepon = nullptr;
	std::unique_ptr<Sprite> sprnumber = nullptr;

	Stage* stage;

	float attackTimer = 0.0f;

	float avoidTimer = 0.0f;
	DirectX::XMFLOAT3 avoidVec = { 0,0,0 };

	float rushSpeed = 200.0f;
	DirectX::XMFLOAT3 rushVec = { 0,0,0 };
	float rushTimer = 0.0f;
	float rushTime = 0.1f;
	float rushDist = 0.0f;
	float rushDistMax = 1.5f;
	bool isChargeRush;

	float chargeTime = 0.0f;
	float chargeValue = 1.0f;
	float chargestil = 0.0f;

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
	DirectX::XMFLOAT3 WeponRootPos{};

	enum class State
	{
		Idle,
		Run,
		Avoid,
		Rush,
		Charge,
		Chain,
		Attack,
	};
	State								state = State::Idle;

	State prevState;

	float hitStopTimer = 0.0f;
};