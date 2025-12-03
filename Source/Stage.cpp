#include "Stage.h"

//コンストラクタ
Stage::Stage()
{
	//ステージのモデルを読み込む
	model = std::make_unique<Model>("Data/Model/Stage/ExampleStage.mdl");

	DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());
}

Stage::~Stage()
{
}

//更新処理
void Stage::Update(float elapsedTime)
{

}

//描画処理
void Stage::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	//レンダラに描画させる
	renderer->Render(rc, transform, model.get(), ShaderId::Lambert);
}
