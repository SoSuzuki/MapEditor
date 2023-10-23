#include "Stage.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "resource.h"

Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage"), hModel_{ -1,-1,-1,-1,-1 },fileName_{ "����.map" }
{
}

Stage::~Stage()
{
}

//������
void Stage::Initialize()
{
    
	std::string modelName[] = {
		"BoxDefault.fbx",
		"BoxBrick.fbx",
		"BoxGrass.fbx",
		"BoxSand.fbx",
		"BoxWater.fbx"
	};
	std::string fnameBase = "Assets/";


    //���f���f�[�^�̃��[�h
    for (int i = 0; i < BLOCK_TYPE::BLOCK_MAX; i++) {
        hModel_[i] = Model::Load(fnameBase + modelName[i]);
        assert(hModel_[i] >= 0);
    }
    
    // table�Ƀu���b�N�̃^�C�v���Z�b�g
    table_.resize(xSize, std::vector<BlockType>(zSize));
    for (int x = 0; x < xSize; x++) {
        for (int z = 0; z < zSize; z++) {
            table_[x][z].bt = DEFAULT;
            table_[x][z].height = 0;

        }
    }

    Load();

}

//�X�V
void Stage::Update()
{
    if (!Input::IsMouseButtonDown(0)) {
        return;
    }
    float w = (float)(Direct3D::scrWidth / 2.0f);
    float h = (float)(Direct3D::scrHeight / 2.0f);
    float offsetX = 0;
    float offsetY = 0;
    float minZ = 0.0f, maxZ = 1.0f;
 
    //�r���[�|�[�g�쐬
    XMMATRIX vp =
    {   //�Q�l�����Fhttps://blog.natade.net/2017/06/09/directx-opengl-viewport/#toc5
        w                ,0                ,0           ,0,
        0                ,-h               ,0           ,0,
        0                ,0                ,maxZ - minZ ,0,
        offsetX + w      ,offsetY + h      ,minZ        ,1
    };

    //�r���[�|�[�g�ϊ�
    XMMATRIX invVP = XMMatrixInverse(nullptr, vp);
    //�v���W�F�N�V�����ϊ�
    XMMATRIX invProj = XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());
    //�r���[�ϊ�
    XMMATRIX invView = XMMatrixInverse(nullptr, Camera::GetViewMatrix());
    XMFLOAT3 mousePosFront;
    XMStoreFloat3(&mousePosFront, Input::GetMousePosition());
    mousePosFront.z = 0.0f;
    XMFLOAT3 mousePosBack;
    XMStoreFloat3(&mousePosBack, Input::GetMousePosition());
    mousePosBack.z = 1.0f;

    //�@ mousePosFront���x�N�g���ɕϊ�
    XMVECTOR vMouseFront = XMLoadFloat3(&mousePosFront);
    //�A �@��invVP,invProj,invView��������
    vMouseFront = XMVector3TransformCoord(vMouseFront, invVP * invProj * invView);
    //�B mousePosBack���x�N�g���ɕϊ�
    XMVECTOR vMouseBack = XMLoadFloat3(&mousePosBack);
    //�C �B��invVP,invProj,invView��������
    vMouseBack = XMVector3TransformCoord(vMouseBack, (invVP * invProj * invView));
    //�D �A����C�Ɍ������ă��C��ł�(�Ƃ肠�������f���ԍ���hModel_[0])
    
    float minDist = 9999.9f;
    int updateX = 0, updateZ = 0;
    bool isHit = false;

    for (int x = 0; x < xSize; x++) {
        for (int z = 0; z < zSize; z++) {
            for (int y = 0; y < table_[x][z].height + 1; y++) {
                RayCastData data;
                data.hit = false;
                XMStoreFloat4(&data.start, vMouseFront);
                XMStoreFloat4(&data.dir, vMouseBack - vMouseFront);
                Transform trans;
                trans.position_.x = x;
                trans.position_.z = z;
                trans.position_.y = y;
                    
                Model::SetTransform(hModel_[0], trans);

                Model::RayCast(hModel_[0], data);

                //�E ���C������������u���[�N�|�C���g�Ŏ~�߂Ċm�F
                if (data.hit) {
                    if (minDist > data.dist) {
                        minDist = data.dist;
                        updateX = x;
                        updateZ = z;
                    }
                    data.hit = false;
                    isHit = true;
                }
            }
        }
    }

    switch (mode_)
    {
    case BLOCK_UP:
        if (isHit)
            table_[updateX][updateZ].height++;
        break;
    case BLOCK_DOWN:
        if (isHit)
            if (table_[updateX][updateZ].height > 0)
                table_[updateX][updateZ].height--;
        break;
    case BLOCK_CHANGE:
        if (isHit)
            table_[updateX][updateZ].bt = (BLOCK_TYPE)hModel_[select_];
        break;
    default:
        break;
    }

    
}

//�`��
void Stage::Draw()
{

    for (int x = 0; x < xSize; x++) {
        for (int z = 0; z < zSize; z++) {
            for (int y = 0; y < table_[x][z].height + 1; y++) {
                int type = table_[x][z].bt;
                Transform bTrans;
                bTrans.position_.x = x;
                bTrans.position_.y = y;
                bTrans.position_.z = z;
                Model::SetTransform(hModel_[type], bTrans);
                Model::Draw(hModel_[type]);

            }
        }
    }

}

void Stage::Release()
{
}

void Stage::SetBlock(int _x, int _z, BLOCK_TYPE _type)
{
    table_[_x][_z].bt = _type;
}

void Stage::SetStackBlock(int _x, int _z, int _height)
{
    table_[_x][_z].height = _height;
}

void Stage::TableSizeChange(int _x, int _z)
{
    xSize = _x;
    zSize = _z;

    table_.resize(xSize, std::vector<BlockType>(zSize));
    for (int x = 0; x < xSize; x++) {
        for (int z = 0; z < zSize; z++) {
            table_[x][z].bt = DEFAULT;
            table_[x][z].height = 0;
        }
    }
}

//�_�C�A���O�p�̃v���V�[�W���i�߂�l��bool�j
// �N���X�Ńv���V�[�W�����g���Ȃ��̂ŁA�U���Ƃ��Ă��łɌĂ΂��悤�ɂ����
// �����N���X�Ŏg�p���Ă��邱�ƂɂȂ�
BOOL Stage::DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		//�_�C�A���O�ł���
	case WM_INITDIALOG:
		// ���W�I�{�^���̏����l
		SendMessage(GetDlgItem(hDlg, IDC_RADIO_UP), BM_SETCHECK, BST_CHECKED, 0);

		// �R���{�{�b�N�X�̏����l
		for (int i = 0; i < BLOCK_TYPE::BLOCK_MAX; i++) {
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_GROUND), CB_ADDSTRING, 0, (LPARAM)blockName[i]);
		}
        SendMessage(GetDlgItem(hDlg, IDC_COMBO_GROUND), CB_SETCURSEL, 0, 0);
		return TRUE;

	case WM_COMMAND:
		// ���W�I�{�^���̐؂�ւ�
        if (IsDlgButtonChecked(hDlg, IDC_RADIO_UP)) {
            mode_ = BLOCK_UP;
            return TRUE;
        }
        if (IsDlgButtonChecked(hDlg, IDC_RADIO_DOWN)) {
            mode_ = BLOCK_DOWN;
            return TRUE;
        }
		if (IsDlgButtonChecked(hDlg, IDC_RADIO_CHANGE)) {
			select_ = SendMessage(GetDlgItem(hDlg, IDC_COMBO_GROUND), CB_GETCURSEL, 0, 0);
			mode_ = BLOCK_CHANGE;
            return TRUE;
		}

		return TRUE;
	}
	return FALSE;
}

void Stage::SizeChange()
{
    //�T�C�Y��ύX�{�^���Ńe�[�u���T�C�Y��ς�����
    //�ς���Ƃ��ɁA�u���܂ł̕ύX�͏����邯�Ǒ��v�H�v�I�Ȋm�F�p�̃_�C�A���O���o���������

    int sizeX = 0, sizeZ = 0;

    //�u�t�@�C����ۑ��v�_�C�A���O�̐ݒ�
    OPENFILENAME ofn;                         	//���O�����ĕۑ��_�C�A���O�̐ݒ�p�\����
    ZeroMemory(&ofn, sizeof(ofn));            	//�\���̏�����
    ofn.lStructSize = sizeof(OPENFILENAME);   	//�\���̂̃T�C�Y
    ofn.lpstrFilter = TEXT("�e�L�X�g�t�@�C��(*.txt)\0*.txt\0")        //�����t�@�C���̎��
        TEXT("���ׂẴt�@�C��(*.*)\0*.*\0\0");     //����
    ofn.lpstrFile = fileName_;               	//�t�@�C����
    ofn.nMaxFile = MAX_PATH;               	    //�p�X�̍ő啶����
    ofn.Flags = OFN_FILEMUSTEXIST;   		    //�t���O�i���݂���t�@�C�������I�ׂȂ��j
    ofn.lpstrDefExt = "txt";                  	//�f�t�H���g�g���q

    //�u�t�@�C����ۑ��v�_�C�A���O
    BOOL selFile;
    selFile = GetOpenFileName(&ofn);

    //�L�����Z�������璆�f
    if (selFile == FALSE) return;

    HANDLE hFile;        //�t�@�C���̃n���h��
    hFile = CreateFile(
        "MapSize.txt",              //�t�@�C����
        GENERIC_READ,           //�A�N�Z�X���[�h�i�ǂݍ��ݗp�j
        0,                      //���L�i�Ȃ��j
        NULL,                   //�Z�L�����e�B�����i�p�����Ȃ��j
        OPEN_EXISTING,          //�쐬���@
        FILE_ATTRIBUTE_NORMAL,  //�����ƃt���O�i�ݒ�Ȃ��j
        NULL);                  //�g�������i�Ȃ��j

    //�t�@�C���̃T�C�Y���擾
    DWORD fileSize = GetFileSize(hFile, NULL);

    //�t�@�C���̃T�C�Y�����������m��
    char* data;
    data = new char[fileSize];

    DWORD dwBytes = 0; //�ǂݍ��݈ʒu

    ReadFile(
        hFile,     //�t�@�C���n���h��
        data,      //�f�[�^������ϐ�
        fileSize,  //�ǂݍ��ރT�C�Y
        &dwBytes,  //�ǂݍ��񂾃T�C�Y
        NULL);     //�I�[�o�[���b�v�h�\���́i����͎g��Ȃ��j

    int index = 0; // �ǂݍ���ł錻�݈ʒu

    std::string buffX = ""; // X�̐��l�f�[�^������
    while (data[index] != ' ' && data[index] != '\n') {
        buffX += data[index];
        index++;
    }
    assert(("Error: Not in the specified format!", buffX[0] != ('X'||'x')));
    index++;
    std::string buffZ = ""; // X�̐��l�f�[�^������
    while (data[index] != ' ' && data[index] != '\0') {
        buffZ += data[index];
        index++;
    }
    assert(("Error: Not in the specified format!", buffZ[0] != ('Z' || 'z')));
    index++;

    CloseHandle(hFile);


    // X��Z�̕����𖳎����Đ�������
    const char* const processX = &buffX[1];
    std::string dataX = "";
    for (int i = 0; i < sizeof(processX); i++) {
        dataX += processX[i];
    }
    std::stoi(dataX, nullptr, 10);

    const char* const processZ = &buffZ[1];
    std::string dataZ = "";
    for (int i = 0; i < sizeof(processZ); i++) {
        dataZ += processZ[i];
    }
    

    TableSizeChange(std::stoi(dataX, nullptr, 10),std::stoi(dataZ, nullptr, 10));
}

void Stage::SaveAsFile()
{
    //�u�t�@�C����ۑ��v�_�C�A���O�̐ݒ�
    OPENFILENAME ofn;                         	//���O�����ĕۑ��_�C�A���O�̐ݒ�p�\����
    ZeroMemory(&ofn, sizeof(ofn));            	//�\���̏�����
    ofn.lStructSize = sizeof(OPENFILENAME);   	//�\���̂̃T�C�Y
    ofn.lpstrFilter = TEXT("�}�b�v�f�[�^(*.map)\0*.map\0")      //�����t�@�C���̎��
        TEXT("���ׂẴt�@�C��(*.*)\0*.*\0\0");                 //����
    ofn.lpstrFile = fileName_;               	//�t�@�C����
    ofn.nMaxFile = MAX_PATH;               	    //�p�X�̍ő啶����
    ofn.Flags = OFN_OVERWRITEPROMPT;   		    //�t���O�i�����t�@�C�������݂�����㏑���m�F�j
    ofn.lpstrDefExt = "map";                  	//�f�t�H���g�g���q

    //�u�t�@�C����ۑ��v�_�C�A���O
    BOOL selFile;
    selFile = GetSaveFileName(&ofn);

    //�L�����Z�������璆�f
    if (selFile == FALSE) return;

    // �Z�[�u�̃��[�`��
#if 0
    HANDLE hFile;        //�t�@�C���̃n���h��
    hFile = CreateFile(
        fileName_,              //�t�@�C����
        GENERIC_WRITE,          //�A�N�Z�X���[�h�i�������ݗp�j
        0,                      //���L�i�Ȃ��j
        NULL,                   //�Z�L�����e�B�����i�p�����Ȃ��j
        CREATE_ALWAYS,          //�쐬���@
        FILE_ATTRIBUTE_NORMAL,  //�����ƃt���O�i�ݒ�Ȃ��j
        NULL);                  //�g�������i�Ȃ��j

    std::stringstream ss;

    for (int z = zSize - 1; z >= 0; z--) {
        for (int x = 0; x < xSize; x++) {
            if (x != xSize - 1) {
                ss << std::bitset<8>(table_[x][z].height) << ' ';
                ss << std::bitset<8>(table_[x][z].bt) << ' ';
            }
            else {
                ss << std::bitset<8>(table_[x][z].height) << ' ';
                ss << std::bitset<8>(table_[x][z].bt) << '\n';
            }
        }
    }

    std::string s = ss.str();

    DWORD dwBytes = 0;  //�������݈ʒu
    WriteFile(
        hFile,                      //�t�@�C���n���h��
        s.c_str(),                  //�ۑ�����f�[�^�i������j
        (DWORD)strlen(s.c_str()),   //�������ޕ�����
        &dwBytes,                   //�������񂾃T�C�Y������ϐ�
        NULL);                      //�I�[�o�[���b�v�h�\���́i����͎g��Ȃ��j
    
    CloseHandle(hFile);

#else
    //�{���̈Ӗ��ł̃o�C�i���t�@�C���œǂݍ��݂���
    std::ofstream fout;


#endif

}

void Stage::Save()
{

// �Z�[�u�̃��[�`��
    HANDLE hFile;        //�t�@�C���̃n���h��
    hFile = CreateFile(
        fileName_,              //�t�@�C����
        GENERIC_WRITE,          //�A�N�Z�X���[�h�i�������ݗp�j
        0,                      //���L�i�Ȃ��j
        NULL,                   //�Z�L�����e�B�����i�p�����Ȃ��j
        CREATE_ALWAYS,          //�쐬���@
        FILE_ATTRIBUTE_NORMAL,  //�����ƃt���O�i�ݒ�Ȃ��j
        NULL);                  //�g�������i�Ȃ��j

    std::stringstream ss;

    for (int z = zSize - 1; z >= 0; z--) {
        for (int x = 0; x < xSize; x++) {
            if (x != xSize - 1) {
                ss << std::bitset<8>(table_[x][z].height) << ' ';
                ss << std::bitset<8>(table_[x][z].bt) << ' ';
            }
            else {
                ss << std::bitset<8>(table_[x][z].height) << ' ';
                ss << std::bitset<8>(table_[x][z].bt) << '\n';
            }
        }
    }
    
    std::string s = ss.str();

    DWORD dwBytes = 0;  //�������݈ʒu
    WriteFile(
        hFile,                      //�t�@�C���n���h��
        s.c_str(),                  //�ۑ�����f�[�^�i������j
        (DWORD)strlen(s.c_str()),   //�������ޕ�����
        &dwBytes,                   //�������񂾃T�C�Y������ϐ�
        NULL);                      //�I�[�o�[���b�v�h�\���́i����͎g��Ȃ��j


    CloseHandle(hFile);
}


void Stage::Load()
{
    //�u�t�@�C����ۑ��v�_�C�A���O�̐ݒ�
    OPENFILENAME ofn;                         	//���O�����ĕۑ��_�C�A���O�̐ݒ�p�\����
    ZeroMemory(&ofn, sizeof(ofn));            	//�\���̏�����
    ofn.lStructSize = sizeof(OPENFILENAME);   	//�\���̂̃T�C�Y
#if 0
    ofn.lpstrFilter = TEXT("�}�b�v�f�[�^(*.map)\0*.map\0")  //�����t�@�C���̎��
        TEXT("���ׂẴt�@�C��(*.*)\0*.*\0\0");             //����
#else
    ofn.lpstrFilter = TEXT("�}�b�v�f�[�^(*.bin)\0*.bin\0")      //�����t�@�C���̎��
    TEXT("���ׂẴt�@�C��(*.*)\0*.*\0\0");                 //����
#endif
    ofn.lpstrFile = fileName_;               	//�t�@�C����
    ofn.nMaxFile = MAX_PATH;               	    //�p�X�̍ő啶����
    ofn.Flags = OFN_FILEMUSTEXIST;   		    //�t���O�i���݂���t�@�C�������I�ׂȂ��j
    ofn.lpstrDefExt = "bin";                  	//�f�t�H���g�g���q

    //�u�t�@�C����ۑ��v�_�C�A���O
    BOOL selFile;
    selFile = GetOpenFileName(&ofn);

    //�L�����Z�������璆�f
    if (selFile == FALSE) return;
    
    HANDLE hFile;        //�t�@�C���̃n���h��
    hFile = CreateFile(
        fileName_,              //�t�@�C����
        GENERIC_READ,           //�A�N�Z�X���[�h�i�ǂݍ��ݗp�j
        0,                      //���L�i�Ȃ��j
        NULL,                   //�Z�L�����e�B�����i�p�����Ȃ��j
        OPEN_EXISTING,          //�쐬���@
        FILE_ATTRIBUTE_NORMAL,  //�����ƃt���O�i�ݒ�Ȃ��j
        NULL);                  //�g�������i�Ȃ��j

    //�t�@�C���̃T�C�Y���擾
    DWORD fileSize = GetFileSize(hFile, NULL);

    //�t�@�C���̃T�C�Y�����������m��
    char* data;
    data = new char[fileSize];

    DWORD dwBytes = 0; //�ǂݍ��݈ʒu

    ReadFile(
        hFile,     //�t�@�C���n���h��
        data,      //�f�[�^������ϐ�
        fileSize,  //�ǂݍ��ރT�C�Y
        &dwBytes,  //�ǂݍ��񂾃T�C�Y
        NULL);     //�I�[�o�[���b�v�h�\���́i����͎g��Ȃ��j

    int index = 0; // �ǂݍ���ł錻�݈ʒu

    for (int z = zSize - 1; z >= 0; z--) {
        for (int x = 0; x < xSize; x++) {
            std::string sBinH = ""; // Height�̃o�C�i���f�[�^(������)������
            while (data[index] != ' ' && data[index] != '\n') {
                sBinH += data[index];
                index++;
            }
            // �o�C�i������10�i���ɕϊ���table_�̊Y�����W�ɑ����index����
            int sIntH = btoi(sBinH);
            SetStackBlock(x, z, sIntH);
            index++;
            std::string sBinB = ""; // BlockType�̃o�C�i���f�[�^(������)������
            while (data[index] != ' ' && data[index] != '\n') {
                sBinB += data[index];
                index++;
            }
            int sIntB = btoi(sBinB);
            SetBlock(x, z, (BLOCK_TYPE)sIntB);
            index++;
        }
        
    }

    CloseHandle(hFile);
}

unsigned int Stage::btoi(const std::string& _bin)
{
    return strtoul(_bin.c_str(), NULL, 2);
}
