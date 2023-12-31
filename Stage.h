#pragma once
#include <Windows.h>    // プロシージャのために必要
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "Engine/GameObject.h"
#include "Engine/Direct3D.h"


namespace {
    int xSize = 15, zSize = 15;//フロアのx,z座標
    const int ySize = 1;
    enum BLOCK_TYPE {
        DEFAULT = 0,
        BRICK,
        GRASS,
        SAND,
        WATER,
        BLOCK_MAX,
    };
    const char* blockName[] = { "デフォルト","レンガ", "草", "砂", "水" };

}

class Stage :
    public GameObject
{
private:
    int hModel_[BLOCK_MAX];     //モデル番号
  //int table_[xSize * zSize];  //フロアの座標
    
    char fileName_[MAX_PATH];  //ファイル名を入れる変数
    char mapSizeName_[MAX_PATH];

    struct BlockType
    {
        BLOCK_TYPE bt;
        int height;
    };
    std::vector<std::vector<BlockType>>table_;
    int blockHeight_;
    int blockType_;

    enum {
        BLOCK_UP = 0,
        BLOCK_DOWN,
        BLOCK_CHANGE,
    };

    int mode_;  // 0:上げる 1:下げる 2:種類を変える
    int select_;// ブロックの種類
   
public:
    //コンストラクタ
    Stage(GameObject* parent);

    //デストラクタ
    ~Stage();

    //初期化
    void Initialize() override;

    //更新
    void Update() override;

    //描画
    void Draw() override;

    //開放
    void Release() override;

    void SetBlock(int _x, int _z, BLOCK_TYPE _type);

    void SetStackBlock(int _x, int _z, int _height);

    // サイズ変更
    void TableSizeChange(int _x, int _z);

    BOOL DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    // サイズ変更
    void SizeChange(HWND _hWnd);

    // 名前を付けて保存
    void SaveAsFile();

    // マップの高さとブロックの種類を保存
    void Save();

    //.mapファイルをロード
    void Load();

};

