#include "Stage.h"
#include <iostream>
#include <sstream>
#include <string>
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "resource.h"

Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage"), hModel_{ -1,-1,-1,-1,-1 },fileName_{ "無題.map" }
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
    float w = (float)(Direct3D::scrWidth / 2.0f);
    float h = (float)(Direct3D::scrHeight / 2.0f);
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
    bool isHit = false;

    for (int x = 0; x < xSize; x++) {
        for (int z = 0; z < zSize; z++) {
            
            for (int y = 0; y < table_[x][z].height + 1; y++) {

                RayCastData data;
                data.hit = false;
                XMStoreFloat4(&data.start, vMouseFront);
                XMStoreFloat4(&data.dir, vMouseBack - vMouseFront);
                Transform trans;
                trans.position_.x = x;
                trans.position_.z = z;
                trans.position_.y = y;
                
                Model::SetTransform(hModel_[0], trans);

                Model::RayCast(hModel_[0], data);

                //⑥ レイが当たったらブレークポイントで止めて確認
                if (data.hit) {
                    if (minDist > data.dist) {
                        minDist = data.dist;
                        updateX = x;
                        updateZ = z;

                    }
                    data.hit = false;
                    isHit = true;
                }
            }
        }
    }

    switch (mode_)
    {
    case BLOCK_UP:
        if (isHit)
            table_[updateX][updateZ].height++;
        break;
    case BLOCK_DOWN:
        if(isHit)
            if (table_[updateX][updateZ].height > 0)
                table_[updateX][updateZ].height--;
        break;
    case BLOCK_CHANGE:
        if (isHit)
            table_[updateX][updateZ].bt = (BLOCK_TYPE)hModel_[select_];
        break;
    default:
        break;
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
        //int i;
        if (IsDlgButtonChecked(hDlg, IDC_RADIO_UP)) {
            mode_ = BLOCK_UP;
            return TRUE;
        }
        if (IsDlgButtonChecked(hDlg, IDC_RADIO_DOWN)) {
            mode_ = BLOCK_DOWN;
            return TRUE;
        }
		if (IsDlgButtonChecked(hDlg, IDC_RADIO_CHANGE)) {
			select_ = SendMessage(GetDlgItem(hDlg, IDC_COMBO_GROUND), CB_GETCURSEL, 0, 0);
			mode_ = BLOCK_CHANGE;
            return TRUE;
		}

		return TRUE;
	}
	return FALSE;
}

void Stage::Save()
{
    //setlocale(LC_ALL, "Japanese");
    

    //「ファイルを保存」ダイアログの設定
    OPENFILENAME ofn;                         	//名前をつけて保存ダイアログの設定用構造体
    ZeroMemory(&ofn, sizeof(ofn));            	//構造体初期化
    ofn.lStructSize = sizeof(OPENFILENAME);   	//構造体のサイズ
    ofn.lpstrFilter = TEXT("マップデータ(*.map)\0*.map\0")        //─┬ファイルの種類
        TEXT("すべてのファイル(*.*)\0*.*\0\0");     //─┘
    ofn.lpstrFile = fileName_;               	//ファイル名
    ofn.nMaxFile = MAX_PATH;               	    //パスの最大文字数
    ofn.Flags = OFN_OVERWRITEPROMPT;   		    //フラグ（同名ファイルが存在したら上書き確認）
    ofn.lpstrDefExt = "map";                  	//デフォルト拡張子

    //「ファイルを保存」ダイアログ
    BOOL selFile;
    selFile = GetSaveFileName(&ofn);

    //キャンセルしたら中断
    if (selFile == FALSE) return;

// セーブのルーチン
    HANDLE hFile;        //ファイルのハンドル
    hFile = CreateFile(
        fileName_,              //ファイル名
        GENERIC_WRITE,          //アクセスモード（書き込み用）
        0,                      //共有（なし）
        NULL,                   //セキュリティ属性（継承しない）
        CREATE_ALWAYS,          //作成方法
        FILE_ATTRIBUTE_NORMAL,  //属性とフラグ（設定なし）
        NULL);                  //拡張属性（なし）

    std::string s = "";

    //for (int z = zSize - 1; z >= 0; z--) {
    //    for (int x = 0; x < xSize; x++) {
    //        if (x != xSize - 1) {
    //            s += std::to_string(table_[x][z].height) + ",";
    //            s += std::to_string(table_[x][z].bt) + ",";
    //        }
    //        else {
    //            s += std::to_string(table_[x][z].height) + ",";
    //            s += std::to_string(table_[x][z].bt);
    //        }
    //    }
    //    s += "\n";
    //}

    for (int z = 0; z < zSize; z++) {
        for (int x = 0; x < xSize; x++) {
            if (x != xSize - 1) {
                s += std::to_string(table_[x][z].height) + ",";
                s += std::to_string(table_[x][z].bt) + ",";
            }
            else {
                s += std::to_string(table_[x][z].height) + ",";
                s += std::to_string(table_[x][z].bt);
            }
        }
    }

    //std::istringstream iss(s);

    DWORD dwBytes = 0;  //書き込み位置
    WriteFile(
        hFile,                      //ファイルハンドル
        s.c_str(),               //保存するデータ（文字列）
        (DWORD)strlen(s.c_str()),//書き込む文字数
        &dwBytes,                   //書き込んだサイズを入れる変数
        NULL);                      //オーバーラップド構造体（今回は使わない）


    CloseHandle(hFile);
}

void Stage::Load()
{
    //「ファイルを保存」ダイアログの設定
    OPENFILENAME ofn;                         	//名前をつけて保存ダイアログの設定用構造体
    ZeroMemory(&ofn, sizeof(ofn));            	//構造体初期化
    ofn.lStructSize = sizeof(OPENFILENAME);   	//構造体のサイズ
    ofn.lpstrFilter = TEXT("マップデータ(*.map)\0*.map\0")        //─┬ファイルの種類
        TEXT("すべてのファイル(*.*)\0*.*\0\0");     //─┘
    ofn.lpstrFile = fileName_;               	//ファイル名
    ofn.nMaxFile = MAX_PATH;               	    //パスの最大文字数
    ofn.Flags = OFN_FILEMUSTEXIST;   		    //フラグ（存在するファイルしか選べない）
    ofn.lpstrDefExt = "map";                  	//デフォルト拡張子

    //「ファイルを保存」ダイアログ
    BOOL selFile;
    selFile = GetOpenFileName(&ofn);

    //キャンセルしたら中断
    if (selFile == FALSE) return;
    
    HANDLE hFile;        //ファイルのハンドル
    hFile = CreateFile(
        fileName_,              //ファイル名
        GENERIC_READ,           //アクセスモード（読み込み用）
        0,                      //共有（なし）
        NULL,                   //セキュリティ属性（継承しない）
        OPEN_EXISTING,          //作成方法
        FILE_ATTRIBUTE_NORMAL,  //属性とフラグ（設定なし）
        NULL);                  //拡張属性（なし）

    //ファイルのサイズを取得
    DWORD fileSize = GetFileSize(hFile, NULL);

    //ファイルのサイズ分メモリを確保
    char* data;
    data = new char[fileSize];

    DWORD dwBytes = 0; //読み込み位置

    ReadFile(
        hFile,     //ファイルハンドル
        data,      //データを入れる変数
        fileSize,  //読み込むサイズ
        &dwBytes,  //読み込んだサイズ
        NULL);     //オーバーラップド構造体（今回は使わない）

    int index = 0; // 読み込んでる現在位置

    for (int x = 0; x < xSize; x++) {
        for (int z = 0; z < zSize; z++) {
            while (data[index] != ',' && data[index] != '\n') {
                
                table_[x][z].height = int(data[index]- '0');
                index++;
            }
            index++;
            while (data[index] != ',' && data[index] != '\n') {

                table_[x][z].bt = (BLOCK_TYPE)(int(data[index] - '0'));
                index++;
            }
            index++;
        }
        
    }

    CloseHandle(hFile);
}
