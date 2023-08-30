#pragma once
#include "Engine/GameObject.h"

namespace {
    const int xSize = 15, zSize = 15;//�t���A��x,z���W
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
    int hModel_[BLOCK_MAX];     //���f���ԍ�
  //int table_[xSize * zSize];  //�t���A�̍��W
    
    struct Stack
    {
        BLOCK_TYPE bt;
        int height;
    }table_[xSize * zSize];

public:
    //�R���X�g���N�^
    Stage(GameObject* parent);

    //�f�X�g���N�^
    ~Stage();

    //������
    void Initialize() override;

    //�X�V
    void Update() override;

    //�`��
    void Draw() override;

    //�J��
    void Release() override;



    void SetBlock(int _x, int _z, BLOCK_TYPE _type);

    void SetStackBlock(int _x, int _z, int _height);
};

