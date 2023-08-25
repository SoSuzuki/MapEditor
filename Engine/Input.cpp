#include "Input.h"


namespace Input
{
	LPDIRECTINPUT8   pDInput = nullptr;
	LPDIRECTINPUTDEVICE8 pKeyDevice = nullptr;
	BYTE keyState[256];			//現在の各キーの状態
	BYTE prevKeyState[256];		//前フレームでの各キーの状態
	XMVECTOR mousePosition;

	void Initialize(HWND hWnd)
	{
		DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDInput, nullptr);
		pDInput->CreateDevice(GUID_SysKeyboard, &pKeyDevice, nullptr);					//デバイスオブジェクトの作成
		pKeyDevice->SetDataFormat(&c_dfDIKeyboard);										//これでキーボードとして扱う
		pKeyDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);	//他アプリに対する入力の優先度
	}

	void Update()
	{
		memcpy(prevKeyState, keyState, sizeof(BYTE) * 256);

		pKeyDevice->Acquire();	//これが無いとキーボードを見失う(?)らしい…
		pKeyDevice->GetDeviceState(sizeof(keyState), &keyState);	//ここでkeystateが更新される
	}

	bool IsKey(int keyCode)
	{
		if (keyState[keyCode] & 0x80)	//10:128, 2:1000 0000
		{
			return true;
		}

		return false;
	}

	bool IsKeyDown(int keyCode)
	{
		//今は押してて、前回は押してない
		if (IsKey(keyCode) && !(prevKeyState[keyCode] & 0x80))
		{
			return true;
		}
		return false;
	}

	bool IsKeyUp(int keyCode)
	{
		//今は押してなくて、前回は押してる
		if (!IsKey(keyCode) && prevKeyState[keyCode] & 0x80)
		{
			return true;
		}
		return false;
		
	}

	XMVECTOR GetMousePosition()
	{
		return mousePosition;
	}

	

	void SetMousePosition(int x, int y)
	{
		mousePosition = XMVectorSet((float)x, (float)y, 0, 0);
	}

	void Release()
	{
		SAFE_RELEASE(pKeyDevice);
		SAFE_RELEASE(pDInput);
	}
}