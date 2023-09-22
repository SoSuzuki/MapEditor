#include "Input.h"
#include <string>

namespace Input
{
	//DirectInputオブジェクト
	LPDIRECTINPUT8   pDInput_ = nullptr;
	
	//キーボード
	LPDIRECTINPUTDEVICE8 pKeyDevice_ = nullptr;
	BYTE keyState_[256];			//現在の各キーの状態
	BYTE prevKeyState_[256];		//前フレームでの各キーの状態
	
	//マウス
	LPDIRECTINPUTDEVICE8	pMouseDevice_;	//デバイスオブジェクト
	DIMOUSESTATE mouseState_;				//マウスの状態
	DIMOUSESTATE prevMouseState_;			//前フレームのマウスの状態
	XMVECTOR mousePosition_;

	void Initialize(HWND hWnd)
	{	
		//DirectInput本体
		DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDInput_, nullptr);
		
		//キーボード
		pDInput_->CreateDevice(GUID_SysKeyboard, &pKeyDevice_, nullptr);					//デバイスオブジェクトの作成
		pKeyDevice_->SetDataFormat(&c_dfDIKeyboard);										//これでキーボードとして扱う
		pKeyDevice_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);	//他アプリに対する入力の優先度
	
		//マウス
		pDInput_->CreateDevice(GUID_SysMouse, &pMouseDevice_, nullptr);
		pMouseDevice_->SetDataFormat(&c_dfDIMouse);
		pMouseDevice_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	}

	void Update()
	{
		//キーボード
		pKeyDevice_->Acquire();	//これが無いとキーボードを見失う(?)らしい…
		memcpy(prevKeyState_, keyState_, sizeof(BYTE) * 256);
		pKeyDevice_->GetDeviceState(sizeof(keyState_), &keyState_);	//ここでkeystateが更新される

		//マウス
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
		//今は押してて、前回は押してない
		if (IsKey(keyCode) && !(prevKeyState_[keyCode] & 0x80))
		{
			return true;
		}
		return false;
	}

	bool IsKeyUp(int keyCode)
	{
		//今は押してなくて、前回は押してる
		if (!IsKey(keyCode) && prevKeyState_[keyCode] & 0x80)
		{
			return true;
		}
		return false;
		
	}

	bool IsMouseButton(int buttonCode)
	{
		//押してる
		if (mouseState_.rgbButtons[buttonCode] & 0x80)
		{
			return true;
		}
		return false;
	}

	bool IsMouseButtonDown(int buttonCode)
	{
		//今は押してて、前回は押してない
		if (IsMouseButton(buttonCode) && !(prevMouseState_.rgbButtons[buttonCode] & 0x80))
		{
			return true;
		}
		return false;
	}

	bool IsMouseButtonUp(int buttonCode)
	{
		//今押してなくて、前回は押してる
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
		/* マウス位置のデバッグ表示 */
		//std::string resStr = std::to_string(x) + "," + std::to_string(y) + "\n";
		//OutputDebugString(resStr.c_str());
	}

	XMFLOAT3 GetMouseMove()
	{
		XMFLOAT3 result = XMFLOAT3((float)mouseState_.lX, (float)mouseState_.lY, (float)mouseState_.lZ);
		return result;
	}

}