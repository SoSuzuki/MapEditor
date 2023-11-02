#pragma once
#include <Windows.h>    // �v���V�[�W���̂��߂ɕK�v
#include <sstream>
#include <fstream>
#include <string>
#include "Engine/GameObject.h"
#include "Engine/Direct3D.h"


namespace {
    int xSize = 15, zSize = 15;//�t���A��x,z���W
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
    char mapSizeName_[MAX_PATH];

    struct BlockType
    {
        BLOCK_TYPE bt;
        int height;
    };
    std::vector<std::vector<BlockType>>table_;
    int blockHeight_;
    int blockType_;

    enum {
        BLOCK_UP = 0,
        BLOCK_DOWN,
        BLOCK_CHANGE,
    };

    int mode_;  // 0:�グ�� 1:������ 2:��ނ�ς���
    int select_;// �u���b�N�̎��
    bool isCheckSelectRange_;    // �͈͑I�����[�h�Ƀ`�F�b�N�������Ă邩�ǂ���

    struct MapIndex {
        int x;
        int z;
        MapIndex(int _x, int _z) { x = _x; z = _z; }
    };
    std::vector<MapIndex> selectCells_;

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

    void SelectRangeCells(MapIndex _start,MapIndex _end);

    // �T�C�Y�ύX
    void TableSizeChange(int _x, int _z);

    BOOL DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    // �T�C�Y�ύX
    void SizeChange(HWND _hWnd);

    // ���O��t���ĕۑ�
    void SaveAsFile();

    // �}�b�v�̍����ƃu���b�N�̎�ނ�ۑ�
    void Save();

    //.map�t�@�C�������[�h
    void Load();

};

