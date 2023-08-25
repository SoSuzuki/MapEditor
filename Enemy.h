#pragma once
#include "Engine/GameObject.h"
class Enemy :
    public GameObject
{
private:
    int hModel_;

public:
    Enemy(GameObject* parent);
    ~Enemy();

    void Initialize()   override;
    void Update()   override;
    void Draw()     override;
    void Release()  override;

    //何かに当たった
    //引数：pTarget 当たった相手
    void OnCollision(GameObject* pTarget) override;
};

