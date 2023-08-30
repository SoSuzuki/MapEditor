#pragma once
#include "Engine/GameObject.h"

namespace {
    const int xSize = 15, zSize = 15;//フロアのx,z座標
    enum BLOCK_TYPE {
        DEFAULT = 0,
        BRICK,
        GRASS,
        SAND,
        WATER,
        BLOCK_MAX,
    };

}

class Stage :
    public GameObject
{
private:
    int hModel_[BLOCK_MAX];     //モデル番号
  //int table_[xSize * zSize];  //フロアの座標
    
    struct Stack
    {
        BLOCK_TYPE bt;
        int height;
    }table_[xSize * zSize];

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
};

