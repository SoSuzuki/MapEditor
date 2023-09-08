#include "Stage.h"
#include <string>
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "resource.h"

Stage::Stage(GameObject* parent)
	:GameObject(parent, "Stage"), hModel_{-1},a_(0)
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

	//���f���f�[�^�̃��[�h
	//hModel_[0] = Model::Load("Assets/BoxDefault.fbx");

	for (int i = 0; i < BLOCK_MAX; i++) {
		hModel_[i] = Model::Load(fname_base + modelName[i]);
		assert(hModel_[i] >= 0);
	}
	// table�Ƀu���b�N�̃^�C�v���Z�b�g
	for (int x = 0; x < xSize; x++) {
		for (int z = 0; z < zSize; z++) {
			SetBlock(x, z, DEFAULT);
			SetStackBlock(x, z, ySize);
		}
	}
}

void Stage::Update()
{
	if (!Input::IsMouseButtonDown(0)) {
		return;
	}
	float w = (float)(scrWidth / 2);
	float h = (float)(scrHeight / 2);
	//OfsetX,Y �� 0
	float minZ = 0.0f, maxZ = 1.0f;
	XMMATRIX vp
	{	//�Q�l�����Fhttps://blog.natade.net/2017/06/09/directx-opengl-viewport/#toc5
		w, 0,	0,	   0,
		0,-h,	0,	   0,
		0, 0,maxZ - minZ,0,
		w, h,	minZ,  1
	};
	//�r���[�|�[�g�ϊ�
	XMMATRIX invVP = XMMatrixInverse(nullptr, vp);
	//�v���W�F�N�V�����ϊ�
	XMMATRIX invProj = XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());
	//�r���[�ϊ�
	XMMATRIX invView = XMMatrixInverse(nullptr, Camera::GetViewMatrix());
	XMFLOAT3 mousePosFront;
	XMStoreFloat3(&mousePosFront, Input::GetMousePosition());
	mousePosFront.z = 0.0f;
	XMFLOAT3 mousePosBack;
	XMStoreFloat3(&mousePosBack, Input::GetMousePosition());
	mousePosBack.z = 1.0f;
	
	//�@ mousePosFront���x�N�g���ɕϊ�
	XMVECTOR vMouseFront = XMLoadFloat3(&mousePosFront);
	//�A �@��invVP,invProj,invView��������
	vMouseFront = XMVector3TransformCoord(vMouseFront, (invVP * invProj * invView));
	//�B mousePosBack���x�N�g���ɕϊ�
	XMVECTOR vMouseBack = XMLoadFloat3(&mousePosBack);
	//�C �B��invVP,invProj,invView��������
	vMouseBack = XMVector3TransformCoord(vMouseBack, (invVP * invProj * invView));
	
	//�D �A����C�Ɍ������ă��C��ł�(�Ƃ肠�������f���ԍ���hModel_[0])
	for (int x = 0; x < xSize; x++) {
		for (int z = 0; z < zSize; z++) {
			for (int y = 0; y < table_[x * z].height + 1; y++) {
				RayCastData data;
				XMStoreFloat4(&data.start, vMouseFront);
				XMStoreFloat4(&data.dir, vMouseBack - vMouseFront);
				Transform trans;
				trans.position_.x = x;
				trans.position_.y = y;
				trans.position_.z = z;
				Model::SetTransform(hModel_[0], trans);

				Model::RayCast(hModel_[0], data);

				//�E ���C������������u���[�N�|�C���g�Ŏ~�߂Ċm�F
				if (data.hit) {
					data.hit = false;
					break;
				}
			}
		}
	}

}

void Stage::Draw()
{
	for (int x = 0; x < xSize; x++)
	{
		for (int z = 0; z < zSize; z++)
		{
			transform_.position_.x = x;
			transform_.position_.z = z;
			

			Model::SetTransform(hModel_[(x + z) % 5], transform_);
			Model::Draw(hModel_[(x + z) % 5]);

			for (int y = 0; y < table_[z * xSize + x].height; y++) {
				transform_.position_.y = y;
				Model::SetTransform(hModel_[(x + z) % 5], transform_);
				Model::Draw(hModel_[(x + z) % 5]);
			}
		
		}
	}
}

void Stage::Release()
{
}

void Stage::SetBlock(int _x, int _z, BLOCK_TYPE _type)
{
	table_[_z * xSize + _x].bt = _type;
}

void Stage::SetStackBlock(int _x, int _z, int _height)
{
	table_[_z * xSize + _x].height = _height;
}

//�_�C�A���O�p�̃v���V�[�W���i�߂�l��bool�j
// �N���X�Ńv���V�[�W�����g���Ȃ��̂ŁA�U���Ƃ��Ă��łɌĂ΂��悤�ɂ����
// �����N���X�Ŏg�p���Ă��邱�ƂɂȂ���
BOOL Stage::DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	//�_�C�A���O�ł���
	case WM_INITDIALOG:
		// ���W�I�{�^���̏����l
		SendMessage(GetDlgItem(hDlg, IDC_RADIO_UP), BM_SETCHECK, BST_CHECKED, 0);

		// �R���{�{�b�N�X�̏����l
		for (int i = 0; i < BLOCK_TYPE::BLOCK_MAX; i++) {
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_GROUND), CB_ADDSTRING, 0, (LPARAM)blockName[i]);
		}
		return TRUE;

	case WM_COMMAND:
		// ���W�I�{�^���̐؂�ւ�
		SendMessage(GetDlgItem(hDlg, IDC_RADIO_UP), CB_GETCURSEL, 0, 0);
		return TRUE;
	}
	return FALSE;
}


