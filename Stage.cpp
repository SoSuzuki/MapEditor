#include "Stage.h"
#include <string>
#include "Engine/Model.h"
#include "resource.h"

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
	for (int x = 0; x < xSize; x++) {
		for (int z = 0; z < zSize; z++) {
			SetBlock(x, z, DEFAULT);
		}
	}
}

void Stage::Update()
{
}

void Stage::Draw()
{
	for (int x = 0; x < xSize; x++)
	{
		for (int z = 0; z < zSize; z++)
		{
			transform_.position_.x = x;
			transform_.position_.z = z;
			
			SetStackBlock(x, z, 3);

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


