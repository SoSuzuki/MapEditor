#pragma once
#include "Engine/GameObject.h"

namespace {
    const int Width = 15, Height = 15;//フロアのx,z座標
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
    int hModel_[BLOCK_MAX];    //モデル番号
    int table_[Width][Height];		//フロアの座標
    

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
};

