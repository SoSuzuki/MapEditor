#include "Stage.h"
#include <string>
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "resource.h"

Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage"), hModel_{ -1,-1,-1,-1,-1 }
{
}

Stage::~Stage()
{
}

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

    // tableにブロックのタイプをセット
    for (int x = 0; x < xSize; x++) {
        for (int z = 0; z < zSize; z++) {
            table_[x][z].bt = DEFAULT;
            table_[x][z].height = 0;

        }
    }

    // SetStackBlock(5, 5, 3); //お試し的に伸ばした

}

//更新
void Stage::Update()
{
    if (!Input::IsMouseButtonDown(0)) {
        return;
    }
    float w = (float)(Direct3D::scrWidth / 2);
    float h = (float)(Direct3D::scrHeight / 2);
    float offsetX = 0;
    float offsetY = 0;
    float minZ = 0.0f, maxZ = 1.0f;
 
    //ビューポート作成
    XMMATRIX vp =
    {   //参考資料：https://blog.natade.net/2017/06/09/directx-opengl-viewport/#toc5
        w                ,0                ,0           ,0,
        0                ,-h               ,0           ,0,
        0                ,0                ,maxZ - minZ ,0,
        offsetX + w      ,offsetY + h      ,minZ        ,1
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
    vMouseFront = XMVector3TransformCoord(vMouseFront, invVP * invProj * invView);
    //③ mousePosBackをベクトルに変換
    XMVECTOR vMouseBack = XMLoadFloat3(&mousePosBack);
    //④ ③にinvVP,invProj,invViewをかける
    vMouseBack = XMVector3TransformCoord(vMouseBack, (invVP * invProj * invView));
    //⑤ ②から④に向かってレイを打つ(とりあえずモデル番号はhModel_[0])
    
    float minDist = 9999.9f;
    int updateX = 0, updateZ = 0;

    for (int x = 0; x < xSize; x++) {
        for (int z = 0; z < zSize; z++) {
            Transform trans;
            trans.position_.x = x;
            trans.position_.z = z;
            for (int y = 0; y < table_[x][z].height + 1; y++) {


                RayCastData data;
                XMStoreFloat4(&data.start, vMouseFront);
                XMStoreFloat4(&data.dir, vMouseBack - vMouseFront);
                
                trans.position_.y = y;
                
                Model::SetTransform(hModel_[0], trans);

                Model::RayCast(hModel_[0], data);

                //⑥ レイが当たったらブレークポイントで止めて確認
                if (data.hit) {
                    if (minDist > data.dist) {
                        minDist = data.dist;
                        updateX = x;
                        updateZ = z;

                        switch (mode_)
                        {
                        case 0:
                            table_[x][z].height++;
                            break;
                        case 1:
                            if (table_[x][z].height > 0)
                                table_[x][z].height--;
                            break;
                        case 2:
                            table_[x][z].bt = (BLOCK_TYPE)hModel_[select_];
                            break;
                        default:
                            break;
                        }
                    }
                    data.hit = false;
                    
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
            for (int y = 0; y < table_[x][z].height + 1; y++) {
                int type = table_[x][z].bt;
                Transform bTrans;
                bTrans.position_.x = x;
                bTrans.position_.y = y;
                bTrans.position_.z = z;
                Model::SetTransform(hModel_[type], bTrans);
                Model::Draw(hModel_[type]);

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
}

void Stage::SetStackBlock(int _x, int _z, int _height)
{
    table_[_x][_z].height = _height;
}

//ダイアログ用のプロシージャ（戻り値はbool）
// クラスでプロシージャが使えないので、偽物としてついでに呼ばれるようにすれば
// 実質クラスで使用していることになる
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
        SendMessage(GetDlgItem(hDlg, IDC_COMBO_GROUND), CB_SETCURSEL, 0, 0);
		return TRUE;

	case WM_COMMAND:
		// ラジオボタンの切り替え
		
		if (IsDlgButtonChecked(hDlg, IDC_RADIO_UP))
			mode_ = 0;

		if (IsDlgButtonChecked(hDlg, IDC_RADIO_DOWN))
			mode_ = 1;

		if (IsDlgButtonChecked(hDlg, IDC_RADIO_CHANGE)) {
			select_ = SendMessage(GetDlgItem(hDlg, IDC_COMBO_GROUND), CB_GETCURSEL, 0, 0);
			mode_ = 2;
		}

		return TRUE;
	}
	return FALSE;
}