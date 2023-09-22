#include "Input.h"
#include <string>

namespace Input
{
	//DirectInput�I�u�W�F�N�g
	LPDIRECTINPUT8   pDInput_ = nullptr;
	
	//�L�[�{�[�h
	LPDIRECTINPUTDEVICE8 pKeyDevice_ = nullptr;
	BYTE keyState_[256];			//���݂̊e�L�[�̏��
	BYTE prevKeyState_[256];		//�O�t���[���ł̊e�L�[�̏��
	
	//�}�E�X
	LPDIRECTINPUTDEVICE8	pMouseDevice_;	//�f�o�C�X�I�u�W�F�N�g
	DIMOUSESTATE mouseState_;				//�}�E�X�̏��
	DIMOUSESTATE prevMouseState_;			//�O�t���[���̃}�E�X�̏��
	XMVECTOR mousePosition_;

	void Initialize(HWND hWnd)
	{	
		//DirectInput�{��
		DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDInput_, nullptr);
		
		//�L�[�{�[�h
		pDInput_->CreateDevice(GUID_SysKeyboard, &pKeyDevice_, nullptr);					//�f�o�C�X�I�u�W�F�N�g�̍쐬
		pKeyDevice_->SetDataFormat(&c_dfDIKeyboard);										//����ŃL�[�{�[�h�Ƃ��Ĉ���
		pKeyDevice_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);	//���A�v���ɑ΂�����̗͂D��x
	
		//�}�E�X
		pDInput_->CreateDevice(GUID_SysMouse, &pMouseDevice_, nullptr);
		pMouseDevice_->SetDataFormat(&c_dfDIMouse);
		pMouseDevice_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	}

	void Update()
	{
		//�L�[�{�[�h
		pKeyDevice_->Acquire();	//���ꂪ�����ƃL�[�{�[�h��������(?)�炵���c
		memcpy(prevKeyState_, keyState_, sizeof(BYTE) * 256);
		pKeyDevice_->GetDeviceState(sizeof(keyState_), &keyState_);	//������keystate���X�V�����

		//�}�E�X
		pMouseDevice_->Acquire();
		memcpy(&prevMouseState_, &mouseState_, sizeof(mouseState_));
		pMouseDevice_->GetDeviceState(sizeof(mouseState_), &mouseState_);
	}

	void Release()
	{
		SAFE_RELEASE(pMouseDevice_);
		SAFE_RELEASE(pKeyDevice_);
		SAFE_RELEASE(pDInput_);
	}

	bool IsKey(int keyCode)
	{
		if (keyState_[keyCode] & 0x80)	//10:128, 2:1000 0000
		{
			return true;
		}

		return false;
	}

	bool IsKeyDown(int keyCode)
	{
		//���͉����ĂāA�O��͉����ĂȂ�
		if (IsKey(keyCode) && !(prevKeyState_[keyCode] & 0x80))
		{
			return true;
		}
		return false;
	}

	bool IsKeyUp(int keyCode)
	{
		//���͉����ĂȂ��āA�O��͉����Ă�
		if (!IsKey(keyCode) && prevKeyState_[keyCode] & 0x80)
		{
			return true;
		}
		return false;
		
	}

	bool IsMouseButton(int buttonCode)
	{
		//�����Ă�
		if (mouseState_.rgbButtons[buttonCode] & 0x80)
		{
			return true;
		}
		return false;
	}

	bool IsMouseButtonDown(int buttonCode)
	{
		//���͉����ĂāA�O��͉����ĂȂ�
		if (IsMouseButton(buttonCode) && !(prevMouseState_.rgbButtons[buttonCode] & 0x80))
		{
			return true;
		}
		return false;
	}

	bool IsMouseButtonUp(int buttonCode)
	{
		//�������ĂȂ��āA�O��͉����Ă�
		if (!IsMouseButton(buttonCode) && prevMouseState_.rgbButtons[buttonCode] & 0x80)
		{
			return true;
		}
		return false;
	}

	XMVECTOR GetMousePosition()
	{
		return mousePosition_;
	}

	void SetMousePosition(int x, int y)
	{
		mousePosition_ = XMVectorSet((float)x, (float)y, 0, 0);
		/* �}�E�X�ʒu�̃f�o�b�O�\�� */
		//std::string resStr = std::to_string(x) + "," + std::to_string(y) + "\n";
		//OutputDebugString(resStr.c_str());
	}

	XMFLOAT3 GetMouseMove()
	{
		XMFLOAT3 result = XMFLOAT3((float)mouseState_.lX, (float)mouseState_.lY, (float)mouseState_.lZ);
		return result;
	}

}