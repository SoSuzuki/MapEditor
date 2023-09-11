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
// 1次元配列Version

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
	//OfsetX,Y は 0
	float minZ = 0.0f, maxZ = 1.0f;
	XMMATRIX vp
	{	//参考資料：https://blog.natade.net/2017/06/09/directx-opengl-viewport/#toc5
		w, 0,	0,	   0,
		0,-h,	0,	   0,
		0, 0,maxZ - minZ,0,
		w, h,	minZ,  1
	};
	//ビューポート変換
	XMMATRIX invVP = XMMatrixInverse(nullptr, vp);
	//プロジェクション変換
	XMMATRIX invProj = XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());
	//ビュー変換
	XMMATRIX invView = XMMatrixInverse(nullptr, Camera::GetViewMatrix());
	XMFLOAT3 mousePosFront;
	XMStoreFloat3(&mousePosFront, Input::GetMousePosition());
	mousePosFront.z = 0.0f;
	XMFLOAT3 mousePosBack;
	XMStoreFloat3(&mousePosBack, Input::GetMousePosition());
	mousePosBack.z = 1.0f;
	
	//① mousePosFrontをベクトルに変換
	XMVECTOR vMouseFront = XMLoadFloat3(&mousePosFront);
	//② ①にinvVP,invProj,invViewをかける
	vMouseFront = XMVector3TransformCoord(vMouseFront, (invVP * invProj * invView));
	//③ mousePosBackをベクトルに変換
	XMVECTOR vMouseBack = XMLoadFloat3(&mousePosBack);
	//④ ③にinvVP,invProj,invViewをかける
	vMouseBack = XMVector3TransformCoord(vMouseBack, (invVP * invProj * invView));
	
	//⑤ ②から④に向かってレイを打つ(とりあえずモデル番号はhModel_[0])
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

				//⑥ レイが当たったらブレークポイントで止めて確認
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
#else // 二次元配列Version
//初期化
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


    //モデルデータのロード
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

//更新
void Stage::Update()
{

    if (Input::IsMouseButtonDown(0)) {

        float w = (float)(Direct3D::scrWidth / 2.0f);
        float h = (float)(Direct3D::scrHeight / 2.0f);
        float offsetX = 0;
        float offsetY = 0;
        float minZ = 0;
        float maxZ = 1;

        //ビューポート作成
        XMMATRIX vp =
        {
            w                ,0                ,0           ,0,
            0                ,-h               ,0           ,0,
            0                ,0                ,maxZ - minZ ,0,
            offsetX + w      ,offsetY + h      ,minZ        ,1
        };

        //ビューポートを逆行列に
        XMMATRIX invVP = XMMatrixInverse(nullptr, vp);
        //プロジェクション変換
        XMMATRIX invProj = XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());
        //びゅー変換
        XMMATRIX invView = XMMatrixInverse(nullptr, Camera::GetViewMatrix());
        XMVECTOR mousePosVec = Input::GetMousePosition();
        XMFLOAT3 mousePosFront;
        XMStoreFloat3(&mousePosFront, mousePosVec);
        mousePosFront.z = 0.0;
        XMVECTOR mouseVec = Input::GetMousePosition();
        XMFLOAT3 mousePosBack;
        XMStoreFloat3(&mousePosBack, mouseVec);
        mousePosBack.z = 1.0f;

        //1,mousePosFrontをベクトルに変換
        XMVECTOR vMouseFront = XMLoadFloat3(&mousePosFront);
        //2. 1にinvVP,invPrj,invViewをかける
        vMouseFront = XMVector3TransformCoord(vMouseFront, invVP * invProj * invView);
        //3,mousePosBackをベクトルに変換
        XMVECTOR vMouseBack = XMLoadFloat3(&mousePosBack);
        //4,3にinvVP,invPrj,invVeewをかける
        vMouseBack = XMVector3TransformCoord(vMouseBack, invVP * invProj * invView);
        //5,2から4に向かってレイを打つ（とりあえず）
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

					//6 レイが当たったらブレークポイント
                    if (data.hit) {
						for (int i=0;i<1;i++) 
						{	// data.distを比較して１番短い奴だけ上げ下げできるようにする

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

//描画
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
    //一次元配列で二次元配列を表現するなら
    //table_[_x * XSIZE +_z] = _type;
    //掛け算から先にするから、x座標が０の時はXSIZEも０になりz座標だけが適用される。xが2とかなら30からとなり3行目ぐらいに入る
}

void Stage::SetStackBlock(int _x, int _z, int _height)
{
    table_[_x][_z].height = _height;
}

#endif

//ダイアログ用のプロシージャ（戻り値はbool）
// クラスでプロシージャが使えないので、偽物としてついでに呼ばれるようにすれば
// 実質クラスで使用していることになるよね
BOOL Stage::DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		//ダイアログできた
	case WM_INITDIALOG:
		// ラジオボタンの初期値
		SendMessage(GetDlgItem(hDlg, IDC_RADIO_UP), BM_SETCHECK, BST_CHECKED, 0);

		// コンボボックスの初期値
		for (int i = 0; i < BLOCK_TYPE::BLOCK_MAX; i++) {
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_GROUND), CB_ADDSTRING, 0, (LPARAM)blockName[i]);
		}
		return TRUE;

	case WM_COMMAND:
		// ラジオボタンの切り替え
		SendMessage(GetDlgItem(hDlg, IDC_RADIO_UP), CB_GETCURSEL, 0, 0);
		return TRUE;
	}
	return FALSE;
}