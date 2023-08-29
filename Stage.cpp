#include "Stage.h"
#include "Engine/Model.h"

Stage::Stage(GameObject* parent)
	:GameObject(parent, "Stage"), hModel_(-1),width_(15),height_(15),table_(nullptr)
{
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
	//���f���f�[�^�̃��[�h
	hModel_ = Model::Load("Assets/BoxDefault.fbx");
	assert(hModel_ >= 0);

	

}

void Stage::Update()
{
}

void Stage::Draw()
{
	Transform blockTrans;	//�Ȃ̂�Transform�^�ŐV�����ϐ������K�v������
	for (int x = 0; x < width_; x++)
	{
		for (int z = 0; z < height_; z++)
		{
			blockTrans.position_.x = x;
			blockTrans.position_.z = z;

			//int type = table_[x][z];
			Model::SetTransform(hModel_, blockTrans);
			Model::Draw(hModel_);
		}
	}
}

void Stage::Release()
{
}
