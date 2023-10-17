//インクルード
#include <Windows.h>
#include <stdlib.h>
#include "Engine/Direct3D.h"
#include "Engine/RootJob.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Camera.h"
#include "DirectXCollision.h"

#include "resource.h"
#include "Stage.h"

#pragma comment(lib, "winmm.lib")

using namespace Camera;

// 定数宣言
const char* WIN_CLASS_NAME = "SampleGame";  //ウィンドウクラス名
const int WINDOW_WIDTH = 800;  //ウィンドウの幅
const int WINDOW_HEIGHT = 600; //ウィンドウの高さ

RootJob* pRootJob = nullptr;

//プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

//エントリーポイント (c++のmain関数みたいなスタート地点)
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	/*******************テストコード**********************/
	//Fbx* pFbx = new Fbx;
	////pFbx->Load("Assets/BoxBrick.fbx");
	//Fbx::RayCastData data;
	//data.start = XMFLOAT4(0, 5, 0,0);
	//data.dir = XMFLOAT4(0, -1, 0, 0);
	//pFbx->RayCast(data);

	//int a=6;	// レイキャストチェック用
	//a++;
	/*****************************************************/

	//ウィンドウクラス（設計図）を作成
	WNDCLASSEX wc;	//このインスタンスに設計項目を詰め込む

	wc.cbSize = sizeof(WNDCLASSEX);				//この構造体のサイズ
	wc.hInstance = hInstance;					//インスタンスハンドル
	wc.lpszClassName = WIN_CLASS_NAME;			//ウィンドウクラス名
	wc.lpfnWndProc = WndProc;					//ウィンドウプロシージャ
	wc.style = CS_VREDRAW | CS_HREDRAW;			//スタイル（デフォルト）
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);	//アイコン
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);	//小さいアイコン
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);	//マウスカーソル
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);//メニュー（アリ。エラー出てるが、
	wc.cbClsExtra = 0;							 //resource.hをインクルードしてるなら気にしない）
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //背景（白）

	RegisterClassEx(&wc);  //クラスを登録	レジスタに入れる

	//ウィンドウサイズの計算
	RECT winRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	//引数(希望するサイズ(RECT構造体)、ウィンドウスタイル、メニューバーの有無)
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, TRUE);	// 第3引数はメニューの有無
	int winW = winRect.right - winRect.left;     //ウィンドウ幅
	int winH = winRect.bottom - winRect.top;     //ウィンドウ高さ

	//ウィンドウを作成
	HWND hWnd = CreateWindow(
		WIN_CLASS_NAME,				//ウィンドウクラス名
		"GameWindow",				//タイトルバーに表示する内容
		WS_OVERLAPPEDWINDOW,		//スタイル（普通のウィンドウ）
		CW_USEDEFAULT,				//表示位置左（おまかせ）
		CW_USEDEFAULT,				//表示位置上（おまかせ）
		winW,						//ウィンドウ幅
		winH,					    //ウィンドウ高さ
		NULL,					    //親ウインドウ（なし）
		NULL,						//メニュー（なし）
		hInstance,					//インスタンス
		NULL			            //パラメータ（なし）
	);

	//ウィンドウを表示
	ShowWindow(hWnd, nCmdShow);

	HRESULT hr;	//ここから下ではhr使えるよ
	//Direct3D初期化
	hr = Direct3D::Initialize(winW, winH, hWnd);
	if (FAILED(hr)) {
		PostQuitMessage(0);	//プログラム終了
	}

	//カメラの初期化
	Camera::Initialize();
	
	//DirectInputの初期化
	Input::Initialize(hWnd);

	pRootJob = new RootJob(nullptr);	//１番上となる親にはnullを入れて、存在しない上を探索させない
	//Objectの初期化
	pRootJob->Initialize();

	// ダイアログボックスを作成
	HWND hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)DialogProc);

	//HWND hDlgM = CreateDialog(hInstance, MAKEINTRESOURCE(IDR_MENU1), hWnd, (DLGPROC)DialogProc);

	//モーダルダイアログ版
	// DialogBox(			);

	//メッセージループ（何か起きるのを待つ）
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)	//WM_QUIT = プログラムが終了したか
	{
		//メッセージあり
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))	//ウィンドウメッセージが１個でもあればプロシージャを呼ぶ
		{
			TranslateMessage(&msg);		//これで下のプロシージャに飛ぶ
			DispatchMessage(&msg);
		}
		
		//メッセージなし
		else
		{
			timeBeginPeriod(1);

			static DWORD countFps = 0;

			static DWORD startTime = timeGetTime();
			DWORD nowTime = timeGetTime();
			static DWORD lastUpdateTime = nowTime;

			if (nowTime - startTime >= 1000)
			{
				char str[16];
				wsprintf(str, "%u", countFps);

				SetWindowText(hWnd, str);

				countFps = 0;
				startTime = nowTime;
			}

			//1000/60だと割り切れないから、左辺に*60して相殺
			if ((nowTime - lastUpdateTime) * 60 <= 1000.0f)
			{
				continue;
			}
			lastUpdateTime = nowTime;

			countFps++;

			timeEndPeriod(1);

			//▼ゲームの処理
			// カメラの更新
			Camera::Update();	//カメラを動かした後に描画するのが正しい
			
			// 入力処理の更新
			Input::Update();

			// RootJobの更新
			pRootJob->UpdateSub();

			//▼描画
			Direct3D::BeginDraw();

			// RootJobから全てのDraw関数を呼ぶ
			pRootJob->DrawSub();

			Direct3D::EndDraw();
			
		}
	}
	Model::Release();
	pRootJob->ReleaseSub();
	SAFE_DELETE(pRootJob);
	Input::Release();
	Direct3D::Release();

	return 0;
}

// WinAPIの仕様上、CALLBACK関数(ダイアログ、ウィンドウに限らず)をクラスに書くことはできない

//ウィンドウプロシージャ（何かあった時によばれる関数）
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_MOUSEMOVE:
		Input::SetMousePosition(LOWORD(lParam), HIWORD(lParam));
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);  //プログラム終了
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);  //プログラム終了
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_MENU_NEW:
			/*OutputDebugString("New File\n");*/
			
			((Stage*)pRootJob->FindObject("Stage"))->NewCreateSave();
			break;
		case ID_MENU_OPEN:
			/*OutputDebugString("Open File\n");*/
			
			((Stage*)pRootJob->FindObject("Stage"))->Load();
			return 0;
		case ID_MENU_SAVE:
			/*OutputDebugString("Save File\n");*/

			//ファイル保存ダイアログで名前を決める
			//決めたファイル名でセーブを実行
			((Stage*)pRootJob->FindObject("Stage"))->Save();
			return 0;
		}

	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//ダイアログ用のプロシージャ（戻り値はbool）
// こちらは本物。何もしない
BOOL CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	// Mainで使われているGameObject型がpRootJobなので使用。
	// FindObjectはシーンの全てを探索するので、検索する側はどれでもいい
	Stage* pStage = (Stage*)pRootJob->FindObject("Stage");
	return pStage->DialogProc(hDlg, msg, wp, lp);
	//		↓1行にまとめると…
	//((Stage*)pRootJob->FindObject("Stage"))->DialogProc(hDlg, msg, wp, lp);
}

