#pragma once
#include <Windows.h>    // �v���V�[�W���̂��߂ɕK�v
#include <iostream>
#include <bitset>
#include <string>
#include "Engine/GameObject.h"
#include "Engine/Direct3D.h"


namespace {
    const int xSize = 15, zSize = 15;//�t���A��x,z���W
    const int ySize = 1;
    enum BLOCK_TYPE {
        DEFAULT = 0,
        BRICK,
        GRASS,
        SAND,
        WATER,
        BLOCK_MAX,
    };
    const char* blockName[] = { "�f�t�H���g","�����K", "��", "��", "��" };

}

class Stage :
    public GameObject
{
private:
    int hModel_[BLOCK_MAX];     //���f���ԍ�
  //int table_[xSize * zSize];  //�t���A�̍��W
    
    char fileName_[MAX_PATH];  //�t�@�C����������ϐ�

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
    int mode_;  // 0:�グ�� 1:������ 2:��ނ�ς���
    int select_;// �u���b�N�̎��
    bool check_;// �`�F�b�N�����Ă��邩

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

    BOOL DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    //�}�b�v�̍����ƃu���b�N�̎�ނ�ۑ�
    void Save();

    //.map�t�@�C�������[�h
    void Load();

    
};

