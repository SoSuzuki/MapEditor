#pragma once
#include "Engine/GameObject.h"
class Stage :
    public GameObject
{
private:
    int hModel_;    //���f���ԍ�
    int** table_;		//�t���A�̍��W
    int width_, height_;//�t���A��x,z���W

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
