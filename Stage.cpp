#include "Stage.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "resource.h"

Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage"), hModel_{ -1,-1,-1,-1,-1 }, fileName_{ "����.bin" }, mapSizeName_{"����.txt"}, 
    blockHeight_(0), blockType_(0)
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
    table_.assign(xSize, std::vector<BlockType>(zSize));
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

                //�͈͑I�����[�h�������Ȃ牺�̏����A�L���Ȃ��̏�����
                if(data.hit) {
                    if(selectCells_.size() < 3) {
                        if (minDist > data.dist) {
                            minDist = data.dist;
                            updateX = x;
                            updateZ = z;
                            selectCells_.push_back(MapIndex(updateX, updateZ));
                        }
                        data.hit = false;
                    }
                    else{
                        SelectRangeCells(selectCells_[0], selectCells_[1]);
                    }
                    isHit = true;
                    
                }
                
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

void Stage::SelectRangeCells(MapIndex _start, MapIndex _end)
{
    // �O.�͈͑I�����[�h��L���������ۂɔ���

    // �@���C�����������Z�����Q���ۑ�����(A�AB�Ɖ���)
    // �AA��B�Ԃ̃Z����List��Vector�Ƀ��X�g�����A�ۑ�
    // �B���̃��X�g�ɓ����Ă���W�ɑ΂���swich���Ŋe�폈�����s��

    for (int z = 0; z < zSize; z++) {
        for (int x = 0; x < xSize; x++) {
            if(/*start��end��������̏ꍇ*/) {
            }
            if (/*start��end���E�����̏ꍇ*/) {

            }
            if (/*start��end���������̏ꍇ*/) {

            }
            if (/*start��end���������̏ꍇ*/) {

            }
        }
    }
}

void Stage::TableSizeChange(int _x, int _z)
{
    xSize = _x;
    zSize = _z;

    table_.assign(xSize, std::vector<BlockType>(zSize));

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

void Stage::SizeChange(HWND _hWnd)
{
    int sizeX = 0, sizeZ = 0;

    //�u�t�@�C����ۑ��v�_�C�A���O�̐ݒ�
    OPENFILENAME ofn;                       	//���O�����ĕۑ��_�C�A���O�̐ݒ�p�\����
    ZeroMemory(&ofn, sizeof(ofn));            	//�\���̏�����
    ofn.lStructSize = sizeof(OPENFILENAME);   	//�\���̂̃T�C�Y
    ofn.lpstrFilter = TEXT("�e�L�X�g�t�@�C��(*.txt)\0*.txt\0")        //�����t�@�C���̎��
        TEXT("���ׂẴt�@�C��(*.*)\0*.*\0\0");                       //����
    ofn.lpstrFile = mapSizeName_;               	//�t�@�C����
    ofn.nMaxFile = MAX_PATH;               	    //�p�X�̍ő啶����
    ofn.Flags = OFN_FILEMUSTEXIST;   		    //�t���O�i���݂���t�@�C�������I�ׂȂ��j
    ofn.lpstrDefExt = "txt";                  	//�f�t�H���g�g���q

    //�u�t�@�C����ۑ��v�_�C�A���O
    BOOL selFile;
    selFile = GetOpenFileName(&ofn);

    //�L�����Z�������璆�f
    if (selFile == FALSE) return;

    //�ύX��ۑ������ɑ��s���邩�m�F �������Œ��f
    if (MessageBox(_hWnd, "Changes will be lost! Do you want to continue?", "CAUTION!", MB_YESNO) == IDNO) 
        return;


    HANDLE hFile;        //�t�@�C���̃n���h��
    hFile = CreateFile(
        mapSizeName_,          //�t�@�C����
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
    std::string buffZ = ""; // Z�̐��l�f�[�^������
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

    const char* const processZ = &buffZ[1];
    std::string dataZ = "";
    for (int i = 0; i < sizeof(processZ); i++) {
        dataZ += processZ[i];
    }
    

    TableSizeChange(std::stoi(dataX, nullptr, 10),std::stoi(dataZ, nullptr, 10));
}

void Stage::SaveAsFile()
{
    //�o�C�i����
    //�u�t�@�C����ۑ��v�_�C�A���O�̐ݒ�
    OPENFILENAME ofn;                         	//���O�����ĕۑ��_�C�A���O�̐ݒ�p�\����
    ZeroMemory(&ofn, sizeof(ofn));            	//�\���̏�����
    ofn.lStructSize = sizeof(OPENFILENAME);   	//�\���̂̃T�C�Y
    ofn.lpstrFilter = TEXT("�}�b�v�f�[�^(*.bin)\0*.bin\0")      //�����t�@�C���̎��
        TEXT("���ׂẴt�@�C��(*.*)\0*.*\0\0");                 //����
    ofn.lpstrFile = fileName_;               	//�t�@�C����
    ofn.nMaxFile = MAX_PATH;               	    //�p�X�̍ő啶����
    ofn.Flags = OFN_OVERWRITEPROMPT;   		    //�t���O�i�����t�@�C�������݂�����㏑���m�F�j
    ofn.lpstrDefExt = "bin";                  	//�f�t�H���g�g���q

    //�u�t�@�C����ۑ��v�_�C�A���O
    BOOL selFile;
    selFile = GetSaveFileName(&ofn);

    //�L�����Z�������璆�f
    if (selFile == FALSE) return;


    std::ofstream ofs(fileName_, std::ios_base::out | std::ios_base::binary);
    //assert(("File open failed", !ofs));
    int block = 0;
    for (int z = zSize - 1; z >= 0; z--) {
        for (int x = 0; x < xSize; x++) {
            ofs.write((const char*)&table_[x][z].height,sizeof(table_[x][z].height));
            block = table_[x][z].bt;
            ofs.write((const char*)&block, sizeof(block));
        }
    }

    ofs.close();
}

void Stage::Save()
{
    std::ofstream ofs(fileName_, std::ios_base::out | std::ios_base::binary);
    //assert(("File open failed", !fout));
    int block = 0;
    for (int z = zSize - 1; z >= 0; z--) {
        for (int x = 0; x < xSize; x++) {
            ofs.write((const char*)&table_[x][z].height, sizeof(table_[x][z].height));
            block = table_[x][z].bt;
            ofs.write((const char*)&block, sizeof(block));
        }
    }
    ofs.close();
}


void Stage::Load()
{
    //�u�t�@�C����ۑ��v�_�C�A���O�̐ݒ�
    OPENFILENAME ofn;                         	//���O�����ĕۑ��_�C�A���O�̐ݒ�p�\����
    ZeroMemory(&ofn, sizeof(ofn));            	//�\���̏�����
    ofn.lStructSize = sizeof(OPENFILENAME);   	//�\���̂̃T�C�Y
    ofn.lpstrFilter = TEXT("�}�b�v�f�[�^(*.bin)\0*.bin\0")  //�����t�@�C���̎��
        TEXT("���ׂẴt�@�C��(*.*)\0*.*\0\0");             //����
    ofn.lpstrFile = fileName_;               	//�t�@�C����
    ofn.nMaxFile = MAX_PATH;               	    //�p�X�̍ő啶����
    ofn.Flags = OFN_FILEMUSTEXIST;   		    //�t���O�i���݂���t�@�C�������I�ׂȂ��j
    ofn.lpstrDefExt = "bin";                  	//�f�t�H���g�g���q

    //�u�t�@�C����ۑ��v�_�C�A���O
    BOOL selFile;
    selFile = GetOpenFileName(&ofn);

    //�L�����Z�������璆�f
    if (selFile == FALSE) return;

    std::ifstream ifs(fileName_, std::ios_base::in | std::ios_base::binary);

    // ��ԍŌ�܂ňړ�
    ifs.seekg(0, std::ios_base::end);
    // ���݂̃|�C���^�ʒu�擾
    int fileSize = ifs.tellg();
    // ��Ԑ擪�Ɉړ�
    ifs.seekg(0, std::ios_base::beg);

    int tableSize = xSize * zSize * sizeof(BlockType);
    // �t�@�C���T�C�Y��(���݂�)�e�[�u���T�C�Y���s��v�̏ꍇ�G���[
    //assert(("X or Z values do not match", fileSize == tableSize));
    if (fileSize != tableSize) {
        int i = MessageBox(0, "X or Z values don't match", 0, MB_OK);
        if (i == IDOK)
            return;
    }

    for (int z = zSize - 1; z >= 0; z--) {
        for (int x = 0; x < xSize; x++) {
            ifs.read((char*)&blockHeight_, sizeof(int));
            table_[x][z].height = blockHeight_;
            ifs.read((char*)&blockType_, sizeof(int));
            table_[x][z].bt = (BLOCK_TYPE)blockType_;
        }
    }

    ifs.close();
}
