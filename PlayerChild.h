#pragma once
#include "Engine/GameObject.h"

class PlayerChild :
    public GameObject
{
private:
    int hModel_;    //���f���ԍ�

public:
    PlayerChild(GameObject* parent);
    ~PlayerChild ();

    void Initialize()   override;
    void Update()   override;
    void Draw()     override;
    void Release()  override;

    //�����ɓ�������
    //�����FpTarget ������������
    void OnCollision(GameObject* pTarget) override;
};

