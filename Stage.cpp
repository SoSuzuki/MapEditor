#include "Stage.h"
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
    table_.resize(xSize, std::vector<BlockType>(zSize));
    for (int x = 0; x < xSize; x++) {
        for (int z = 0; z < zSize; z++) {
            table_[x][z].bt = DEFAULT;
            table_[x][z].height = 0;

        }
    }

    Load();

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
        if (isHit)
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

void Stage::TableSizeChange(int _x, int _z)
{
    xSize = _x;
    zSize = _z;

    table_.resize(xSize, std::vector<BlockType>(zSize));
    for (int x = 0; x < xSize; x++) {
        for (int z = 0; z < zSize; z++) {
            table_[x][z].bt = DEFAULT;
            table_[x][z].height = 0;
        }
    }
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

void Stage::SizeChange()
{
    //サイズを変更ボタンでテーブルサイズを変えたい
    //変えるときに、「今までの変更は消えるけど大丈夫？」的な確認用のダイアログを出したいよね

    int sizeX = 0, sizeZ = 0;

    //「ファイルを保存」ダイアログの設定
    OPENFILENAME ofn;                         	//名前をつけて保存ダイアログの設定用構造体
    ZeroMemory(&ofn, sizeof(ofn));            	//構造体初期化
    ofn.lStructSize = sizeof(OPENFILENAME);   	//構造体のサイズ
    ofn.lpstrFilter = TEXT("テキストファイル(*.txt)\0*.txt\0")        //─┬ファイルの種類
        TEXT("すべてのファイル(*.*)\0*.*\0\0");     //─┘
    ofn.lpstrFile = fileName_;               	//ファイル名
    ofn.nMaxFile = MAX_PATH;               	    //パスの最大文字数
    ofn.Flags = OFN_FILEMUSTEXIST;   		    //フラグ（存在するファイルしか選べない）
    ofn.lpstrDefExt = "txt";                  	//デフォルト拡張子

    //「ファイルを保存」ダイアログ
    BOOL selFile;
    selFile = GetOpenFileName(&ofn);

    //キャンセルしたら中断
    if (selFile == FALSE) return;

    HANDLE hFile;        //ファイルのハンドル
    hFile = CreateFile(
        "MapSize.txt",              //ファイル名
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

    std::string buffX = ""; // Xの数値データを入れる
    while (data[index] != ' ' && data[index] != '\n') {
        buffX += data[index];
        index++;
    }
    assert(("Error: Not in the specified format!", buffX[0] != ('X'||'x')));
    index++;
    std::string buffZ = ""; // Xの数値データを入れる
    while (data[index] != ' ' && data[index] != '\0') {
        buffZ += data[index];
        index++;
    }
    assert(("Error: Not in the specified format!", buffZ[0] != ('Z' || 'z')));
    index++;

    CloseHandle(hFile);


    // XとZの文字を無視して数字だけ
    const char* const processX = &buffX[1];
    std::string dataX = "";
    for (int i = 0; i < sizeof(processX); i++) {
        dataX += processX[i];
    }
    std::stoi(dataX, nullptr, 10);

    const char* const processZ = &buffZ[1];
    std::string dataZ = "";
    for (int i = 0; i < sizeof(processZ); i++) {
        dataZ += processZ[i];
    }
    

    TableSizeChange(std::stoi(dataX, nullptr, 10),std::stoi(dataZ, nullptr, 10));
}

void Stage::SaveAsFile()
{
    //「ファイルを保存」ダイアログの設定
    OPENFILENAME ofn;                         	//名前をつけて保存ダイアログの設定用構造体
    ZeroMemory(&ofn, sizeof(ofn));            	//構造体初期化
    ofn.lStructSize = sizeof(OPENFILENAME);   	//構造体のサイズ
    ofn.lpstrFilter = TEXT("マップデータ(*.map)\0*.map\0")      //─┬ファイルの種類
        TEXT("すべてのファイル(*.*)\0*.*\0\0");                 //─┘
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
#if 0
    HANDLE hFile;        //ファイルのハンドル
    hFile = CreateFile(
        fileName_,              //ファイル名
        GENERIC_WRITE,          //アクセスモード（書き込み用）
        0,                      //共有（なし）
        NULL,                   //セキュリティ属性（継承しない）
        CREATE_ALWAYS,          //作成方法
        FILE_ATTRIBUTE_NORMAL,  //属性とフラグ（設定なし）
        NULL);                  //拡張属性（なし）

    std::stringstream ss;

    for (int z = zSize - 1; z >= 0; z--) {
        for (int x = 0; x < xSize; x++) {
            if (x != xSize - 1) {
                ss << std::bitset<8>(table_[x][z].height) << ' ';
                ss << std::bitset<8>(table_[x][z].bt) << ' ';
            }
            else {
                ss << std::bitset<8>(table_[x][z].height) << ' ';
                ss << std::bitset<8>(table_[x][z].bt) << '\n';
            }
        }
    }

    std::string s = ss.str();

    DWORD dwBytes = 0;  //書き込み位置
    WriteFile(
        hFile,                      //ファイルハンドル
        s.c_str(),                  //保存するデータ（文字列）
        (DWORD)strlen(s.c_str()),   //書き込む文字数
        &dwBytes,                   //書き込んだサイズを入れる変数
        NULL);                      //オーバーラップド構造体（今回は使わない）
    
    CloseHandle(hFile);

#else
    //本当の意味でのバイナリファイルで読み込みたい
    std::ofstream fout;


#endif

}

void Stage::Save()
{

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

    std::stringstream ss;

    for (int z = zSize - 1; z >= 0; z--) {
        for (int x = 0; x < xSize; x++) {
            if (x != xSize - 1) {
                ss << std::bitset<8>(table_[x][z].height) << ' ';
                ss << std::bitset<8>(table_[x][z].bt) << ' ';
            }
            else {
                ss << std::bitset<8>(table_[x][z].height) << ' ';
                ss << std::bitset<8>(table_[x][z].bt) << '\n';
            }
        }
    }
    
    std::string s = ss.str();

    DWORD dwBytes = 0;  //書き込み位置
    WriteFile(
        hFile,                      //ファイルハンドル
        s.c_str(),                  //保存するデータ（文字列）
        (DWORD)strlen(s.c_str()),   //書き込む文字数
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
#if 0
    ofn.lpstrFilter = TEXT("マップデータ(*.map)\0*.map\0")  //─┬ファイルの種類
        TEXT("すべてのファイル(*.*)\0*.*\0\0");             //─┘
#else
    ofn.lpstrFilter = TEXT("マップデータ(*.bin)\0*.bin\0")      //─┬ファイルの種類
    TEXT("すべてのファイル(*.*)\0*.*\0\0");                 //─┘
#endif
    ofn.lpstrFile = fileName_;               	//ファイル名
    ofn.nMaxFile = MAX_PATH;               	    //パスの最大文字数
    ofn.Flags = OFN_FILEMUSTEXIST;   		    //フラグ（存在するファイルしか選べない）
    ofn.lpstrDefExt = "bin";                  	//デフォルト拡張子

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

    for (int z = zSize - 1; z >= 0; z--) {
        for (int x = 0; x < xSize; x++) {
            std::string sBinH = ""; // Heightのバイナリデータ(文字列)を入れる
            while (data[index] != ' ' && data[index] != '\n') {
                sBinH += data[index];
                index++;
            }
            // バイナリから10進数に変換→table_の該当座標に代入→index増分
            int sIntH = btoi(sBinH);
            SetStackBlock(x, z, sIntH);
            index++;
            std::string sBinB = ""; // BlockTypeのバイナリデータ(文字列)を入れる
            while (data[index] != ' ' && data[index] != '\n') {
                sBinB += data[index];
                index++;
            }
            int sIntB = btoi(sBinB);
            SetBlock(x, z, (BLOCK_TYPE)sIntB);
            index++;
        }
        
    }

    CloseHandle(hFile);
}

unsigned int Stage::btoi(const std::string& _bin)
{
    return strtoul(_bin.c_str(), NULL, 2);
}
