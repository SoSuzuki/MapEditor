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


