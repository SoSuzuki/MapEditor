#pragma once
#include "Engine/GameObject.h"

namespace {
    const int Width = 15, Height = 15;//�t���A��x,z���W
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
    int hModel_[BLOCK_MAX];    //���f���ԍ�
    int table_[Width][Height];		//�t���A�̍��W
    

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
};

