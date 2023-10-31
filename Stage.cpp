#include "Stage.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "resource.h"

Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage"), hModel_{ -1,-1,-1,-1,-1 }, fileName_{ "無題.bin" }, mapSizeName_{"無題.txt"}, 
    blockHeight_(0), blockType_(0)
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
    table_.assign(xSize, std::vector<BlockType>(zSize));
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

                //範囲選択モードが無効なら下の処理、有効なら上の処理に
                if(data.hit) {
                    if(selectCells_.size() < 3) {
                        if (minDist > data.dist) {
                            minDist = data.dist;
                            updateX = x;
                            updateZ = z;
                            selectCells_.push_back(MapIndex(updateX, updateZ));
                        }
                        data.hit = false;
                    }
                    else{
                        SelectRangeCells(selectCells_[0], selectCells_[1]);
                    }
                    isHit = true;
                    
                }
                
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

void Stage::SelectRangeCells(MapIndex _start, MapIndex _end)
{
    // ０.範囲選択モードを有効化した際に発動

    // ①レイが当たったセルを２個分保存する(A、Bと仮定)
    // ②A→B間のセルをListかVectorにリスト化し、保存
    // ③↑のリストに入ってる座標に対してswich文で各種処理を行う

    for (int z = 0; z < zSize; z++) {
        for (int x = 0; x < xSize; x++) {
            if(/*start→endが上方向の場合*/) {
            }
            if (/*start→endが右方向の場合*/) {

            }
            if (/*start→endが下方向の場合*/) {

            }
            if (/*start→endが左方向の場合*/) {

            }
        }
    }
}

void Stage::TableSizeChange(int _x, int _z)
{
    xSize = _x;
    zSize = _z;

    table_.assign(xSize, std::vector<BlockType>(zSize));

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

void Stage::SizeChange(HWND _hWnd)
{
    int sizeX = 0, sizeZ = 0;

    //「ファイルを保存」ダイアログの設定
    OPENFILENAME ofn;                       	//名前をつけて保存ダイアログの設定用構造体
    ZeroMemory(&ofn, sizeof(ofn));            	//構造体初期化
    ofn.lStructSize = sizeof(OPENFILENAME);   	//構造体のサイズ
    ofn.lpstrFilter = TEXT("テキストファイル(*.txt)\0*.txt\0")        //─┬ファイルの種類
        TEXT("すべてのファイル(*.*)\0*.*\0\0");                       //─┘
    ofn.lpstrFile = mapSizeName_;               	//ファイル名
    ofn.nMaxFile = MAX_PATH;               	    //パスの最大文字数
    ofn.Flags = OFN_FILEMUSTEXIST;   		    //フラグ（存在するファイルしか選べない）
    ofn.lpstrDefExt = "txt";                  	//デフォルト拡張子

    //「ファイルを保存」ダイアログ
    BOOL selFile;
    selFile = GetOpenFileName(&ofn);

    //キャンセルしたら中断
    if (selFile == FALSE) return;

    //変更を保存せずに続行するか確認 いいえで中断
    if (MessageBox(_hWnd, "Changes will be lost! Do you want to continue?", "CAUTION!", MB_YESNO) == IDNO) 
        return;


    HANDLE hFile;        //ファイルのハンドル
    hFile = CreateFile(
        mapSizeName_,          //ファイル名
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
    std::string buffZ = ""; // Zの数値データを入れる
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

    const char* const processZ = &buffZ[1];
    std::string dataZ = "";
    for (int i = 0; i < sizeof(processZ); i++) {
        dataZ += processZ[i];
    }
    

    TableSizeChange(std::stoi(dataX, nullptr, 10),std::stoi(dataZ, nullptr, 10));
}

void Stage::SaveAsFile()
{
    //バイナリ編
    //「ファイルを保存」ダイアログの設定
    OPENFILENAME ofn;                         	//名前をつけて保存ダイアログの設定用構造体
    ZeroMemory(&ofn, sizeof(ofn));            	//構造体初期化
    ofn.lStructSize = sizeof(OPENFILENAME);   	//構造体のサイズ
    ofn.lpstrFilter = TEXT("マップデータ(*.bin)\0*.bin\0")      //─┬ファイルの種類
        TEXT("すべてのファイル(*.*)\0*.*\0\0");                 //─┘
    ofn.lpstrFile = fileName_;               	//ファイル名
    ofn.nMaxFile = MAX_PATH;               	    //パスの最大文字数
    ofn.Flags = OFN_OVERWRITEPROMPT;   		    //フラグ（同名ファイルが存在したら上書き確認）
    ofn.lpstrDefExt = "bin";                  	//デフォルト拡張子

    //「ファイルを保存」ダイアログ
    BOOL selFile;
    selFile = GetSaveFileName(&ofn);

    //キャンセルしたら中断
    if (selFile == FALSE) return;


    std::ofstream ofs(fileName_, std::ios_base::out | std::ios_base::binary);
    //assert(("File open failed", !ofs));
    int block = 0;
    for (int z = zSize - 1; z >= 0; z--) {
        for (int x = 0; x < xSize; x++) {
            ofs.write((const char*)&table_[x][z].height,sizeof(table_[x][z].height));
            block = table_[x][z].bt;
            ofs.write((const char*)&block, sizeof(block));
        }
    }

    ofs.close();
}

void Stage::Save()
{
    std::ofstream ofs(fileName_, std::ios_base::out | std::ios_base::binary);
    //assert(("File open failed", !fout));
    int block = 0;
    for (int z = zSize - 1; z >= 0; z--) {
        for (int x = 0; x < xSize; x++) {
            ofs.write((const char*)&table_[x][z].height, sizeof(table_[x][z].height));
            block = table_[x][z].bt;
            ofs.write((const char*)&block, sizeof(block));
        }
    }
    ofs.close();
}


void Stage::Load()
{
    //「ファイルを保存」ダイアログの設定
    OPENFILENAME ofn;                         	//名前をつけて保存ダイアログの設定用構造体
    ZeroMemory(&ofn, sizeof(ofn));            	//構造体初期化
    ofn.lStructSize = sizeof(OPENFILENAME);   	//構造体のサイズ
    ofn.lpstrFilter = TEXT("マップデータ(*.bin)\0*.bin\0")  //─┬ファイルの種類
        TEXT("すべてのファイル(*.*)\0*.*\0\0");             //─┘
    ofn.lpstrFile = fileName_;               	//ファイル名
    ofn.nMaxFile = MAX_PATH;               	    //パスの最大文字数
    ofn.Flags = OFN_FILEMUSTEXIST;   		    //フラグ（存在するファイルしか選べない）
    ofn.lpstrDefExt = "bin";                  	//デフォルト拡張子

    //「ファイルを保存」ダイアログ
    BOOL selFile;
    selFile = GetOpenFileName(&ofn);

    //キャンセルしたら中断
    if (selFile == FALSE) return;

    std::ifstream ifs(fileName_, std::ios_base::in | std::ios_base::binary);

    // 一番最後まで移動
    ifs.seekg(0, std::ios_base::end);
    // 現在のポインタ位置取得
    int fileSize = ifs.tellg();
    // 一番先頭に移動
    ifs.seekg(0, std::ios_base::beg);

    int tableSize = xSize * zSize * sizeof(BlockType);
    // ファイルサイズと(現在の)テーブルサイズが不一致の場合エラー
    //assert(("X or Z values do not match", fileSize == tableSize));
    if (fileSize != tableSize) {
        int i = MessageBox(0, "X or Z values don't match", 0, MB_OK);
        if (i == IDOK)
            return;
    }

    for (int z = zSize - 1; z >= 0; z--) {
        for (int x = 0; x < xSize; x++) {
            ifs.read((char*)&blockHeight_, sizeof(int));
            table_[x][z].height = blockHeight_;
            ifs.read((char*)&blockType_, sizeof(int));
            table_[x][z].bt = (BLOCK_TYPE)blockType_;
        }
    }

    ifs.close();
}
