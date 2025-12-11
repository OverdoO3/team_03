#include "Stage.h"

//コンストラクタ
Stage::Stage()
{
	//ステージのモデルを読み込む
	model = std::make_unique<Model>("Data/Model/Stage/demostage.mdl");

	scale = { 0.01f,0.01f,0.01f };
	angle = { 0,30,0 };
	UpdateTransfomEuler();
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
