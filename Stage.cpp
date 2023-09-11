#include "Stage.h"
#include <string>
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "resource.h"

Stage::Stage(GameObject* parent)
	:GameObject(parent, "Stage"), hModel_{ -1 }
{
}

Stage::~Stage()
{
}

#if 0
// 1�����z��Version

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
					table_[x * z].height++;
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
#else // �񎟌��z��Version
//������
void Stage::Initialize()
{

	std::string modelName[] = {
		"BoxDefault.fbx",
		"BoxBrick.fbx",
		"BoxGrass.fbx",
		"BoxSand.fbx",
		"BoxWater.fbx"
	};
	std::string fnameBase = "Assets/";


    //���f���f�[�^�̃��[�h
    for (int i = 0; i < BLOCK_TYPE::BLOCK_MAX; i++) {
        hModel_[i] = Model::Load(fnameBase + modelName[i]);
        assert(hModel_[i] >= 0);
    }

    for (int x = 0; x < xSize; x++) {
        for (int z = 0; z < zSize; z++) {
            table_[x][z].bt = DEFAULT;
            table_[x][z].height = 0;

        }
    }

    SetStackBlock(5, 5, 3);

}

//�X�V
void Stage::Update()
{

    if (Input::IsMouseButtonDown(0)) {

        float w = (float)(Direct3D::scrWidth / 2.0f);
        float h = (float)(Direct3D::scrHeight / 2.0f);
        float offsetX = 0;
        float offsetY = 0;
        float minZ = 0;
        float maxZ = 1;

        //�r���[�|�[�g�쐬
        XMMATRIX vp =
        {
            w                ,0                ,0           ,0,
            0                ,-h               ,0           ,0,
            0                ,0                ,maxZ - minZ ,0,
            offsetX + w      ,offsetY + h      ,minZ        ,1
        };

        //�r���[�|�[�g���t�s���
        XMMATRIX invVP = XMMatrixInverse(nullptr, vp);
        //�v���W�F�N�V�����ϊ�
        XMMATRIX invProj = XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());
        //�т�[�ϊ�
        XMMATRIX invView = XMMatrixInverse(nullptr, Camera::GetViewMatrix());
        XMVECTOR mousePosVec = Input::GetMousePosition();
        XMFLOAT3 mousePosFront;
        XMStoreFloat3(&mousePosFront, mousePosVec);
        mousePosFront.z = 0.0;
        XMVECTOR mouseVec = Input::GetMousePosition();
        XMFLOAT3 mousePosBack;
        XMStoreFloat3(&mousePosBack, mouseVec);
        mousePosBack.z = 1.0f;

        //1,mousePosFront���x�N�g���ɕϊ�
        XMVECTOR vMouseFront = XMLoadFloat3(&mousePosFront);
        //2. 1��invVP,invPrj,invView��������
        vMouseFront = XMVector3TransformCoord(vMouseFront, invVP * invProj * invView);
        //3,mousePosBack���x�N�g���ɕϊ�
        XMVECTOR vMouseBack = XMLoadFloat3(&mousePosBack);
        //4,3��invVP,invPrj,invVeew��������
        vMouseBack = XMVector3TransformCoord(vMouseBack, invVP * invProj * invView);
        //5,2����4�Ɍ������ă��C��łi�Ƃ肠�����j
        for (int x = 0; x < 15; x++) {
            for (int z = 0; z < 15; z++) {
                for (int y = 0; y < table_[x][z].height + 1; y++) {

                    RayCastData data;
                    XMStoreFloat4(&data.start, vMouseFront);
                    XMStoreFloat4(&data.dir, vMouseBack - vMouseFront);
                    Transform trans;
                    trans.position_.x = x;
                    trans.position_.y = y;
                    trans.position_.z = z;
                    Model::SetTransform(hModel_[0], trans);

                    Model::RayCast(hModel_[0], data);

					//6 ���C������������u���[�N�|�C���g
                    if (data.hit) {
						for (int i=0;i<1;i++) 
						{	// data.dist���r���ĂP�ԒZ���z�����グ�����ł���悤�ɂ���

						}


                        table_[x][z].height++;
                        data.hit = false;
                        return;
                    }
                }
            }
        }
    }
}

//�`��
void Stage::Draw()
{

    for (int x = 0; x < xSize; x++) {
        for (int z = 0; z < zSize; z++) {
			transform_.position_.x = x;
			transform_.position_.z = z;
            for (int y = 0; y < table_[x][z].height + 1; y++) {
                int type = table_[x][z].bt;
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
    table_[_x][_z].bt = _type;
    //�ꎟ���z��œ񎟌��z���\������Ȃ�
    //table_[_x * XSIZE +_z] = _type;
    //�|���Z�����ɂ��邩��Ax���W���O�̎���XSIZE���O�ɂȂ�z���W�������K�p�����Bx��2�Ƃ��Ȃ�30����ƂȂ�3�s�ڂ��炢�ɓ���
}

void Stage::SetStackBlock(int _x, int _z, int _height)
{
    table_[_x][_z].height = _height;
}

#endif

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