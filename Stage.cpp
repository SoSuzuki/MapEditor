#include "Stage.h"
#include "Engine/Model.h"

Stage::Stage(GameObject* parent)
	:GameObject(parent, "Player"), hModel_(-1)
{
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
	//モデルデータのロード
	hModel_ = Model::Load("Assets/BoxDefault.fbx");
	assert(hModel_ >= 0);
}

void Stage::Update()
{
}

void Stage::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void Stage::Release()
{
}
