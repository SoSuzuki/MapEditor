#include "Stage.h"
#include <string>
#include "Engine/Model.h"

Stage::Stage(GameObject* parent)
	:GameObject(parent, "Stage"), hModel_{-1}
{
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
	std::string modelName[] = {
		"BoxDefault.fbx",
		"BoxBrick.fbx",
		"BoxGrass.fbx",
		"BoxSand.fbx",
		"BoxWater.fbx"
	};
	std::string fname_base = "Assets/";

	//モデルデータのロード
	//hModel_[0] = Model::Load("Assets/BoxDefault.fbx");

	for (int i = 0; i < BLOCK_MAX; i++) {
		hModel_[i] = Model::Load(fname_base + modelName[i]);
		assert(hModel_[i] >= 0);
	}
	// tableにブロックのタイプをセット
	for (int z = 0; z < Height; z++) {
		for (int x = 0; x < Width; x++) {
			table_[x][z] = hModel_[DEFAULT];
		}
	}

}

void Stage::Update()
{
}

void Stage::Draw()
{
	for (int x = 0; x < Width; x++)
	{
		for (int z = 0; z < Height; z++)
		{
			transform_.position_.x = x;
			transform_.position_.z = z;

			//Model::SetTransform(hModel_[BLOCK_TYPE::DEFAULT], BLOCK_TYPETrans);
			//Model::Draw(hModel_[BLOCK_TYPE::DEFAULT]);
			Model::SetTransform(hModel_[(x + z) % 5], transform_);
			Model::Draw(hModel_[(x + z) % 5]);
		}
	}
}

void Stage::Release()
{
}
