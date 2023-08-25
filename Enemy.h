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

    //�����ɓ�������
    //�����FpTarget ������������
    void OnCollision(GameObject* pTarget) override;
};

