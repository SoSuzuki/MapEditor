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

    //‰½‚©‚É“–‚½‚Á‚½
    //ˆø”FpTarget “–‚½‚Á‚½‘Šè
    void OnCollision(GameObject* pTarget) override;
};

