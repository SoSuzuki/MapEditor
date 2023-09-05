#pragma once

#include <dInput.h>
#include <DirectXMath.h>

using namespace DirectX;

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")	//Input�Ɋւ��Ă͑��X�ς�邱�Ƃ��Ȃ�����W

//����1�s�̂��߂ɑ��̃w�b�_�[���C���N���[�h����͖̂��ʂ�����������
#define SAFE_RELEASE(p) if(p != nullptr){ p->Release(); p = nullptr;}

namespace Input
{
	void Initialize(HWND hWnd);
	void Update();
	void Release();

/*******************�L�[�{�[�h***********************/
	bool IsKey(int keyCode);
	bool IsKeyDown(int keyCode);
	bool IsKeyUp(int keyCode);
	

/*********************�}�E�X*************************/
	//�}�E�X�̃{�^����������Ă��邩���ׂ�
	//�����FbuttonCode	���ׂ����{�^���̔ԍ�
	//�ߒl�F������Ă����true
	bool IsMouseButton(int buttonCode);

	//�}�E�X�̃{�^�����������������ׂ�i�������ςȂ��͖����j
	//�����FbuttonCode	���ׂ����{�^���̔ԍ�
	//�ߒl�F�������u�Ԃ�������true
	bool IsMouseButtonDown(int buttonCode);

	//�}�E�X�̃{�^�����������������ׂ�
	//�����FbuttonCode	���ׂ����{�^���̔ԍ�
	//�ߒl�F�������u�Ԃ�������true
	bool IsMouseButtonUp(int buttonCode);


	//�}�E�X�J�[�\���̈ʒu���擾
	//�ߒl�F�}�E�X�J�[�\���̈ʒu
	XMVECTOR GetMousePosition();

	//�}�E�X�J�[�\���̈ʒu���Z�b�g
	//�����F�}�E�X�J�[�\���̈ʒu
	void SetMousePosition(int x, int y);

	//���̃t���[���ł̃}�E�X�̈ړ��ʂ��擾
	//�ߒl�FX,Y �}�E�X�̈ړ��ʁ@�^�@Z,�z�C�[���̉�]��
	XMFLOAT3 GetMouseMove();

};