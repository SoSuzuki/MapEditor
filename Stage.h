#pragma once
#include <Windows.h>    // プロシージャのために必要
#include <iostream>
#include <bitset>
#include <string>
#include "Engine/GameObject.h"
#include "Engine/Direct3D.h"


namespace {
    const int xSize = 15, zSize = 15;//フロアのx,z座標
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

#if 0
    struct Stack
    {
        BLOCK_TYPE bt;
        int height;
    }table_[xSize * zSize];
#else
    struct BlockType
    {
        BLOCK_TYPE bt;
        int height;
    }table_[xSize][zSize];

    enum {
        BLOCK_UP = 0,
        BLOCK_DOWN,
        BLOCK_CHANGE,
    };
#endif
    int mode_;  // 0:上げる 1:下げる 2:種類を変える
    int select_;// ブロックの種類
    bool check_;// チェックがついているか

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

    BOOL DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    //マップの高さとブロックの種類を保存
    void Save();

    //.mapファイルをロード
    void Load();

    
};

