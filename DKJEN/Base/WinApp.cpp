#include"WinApp.h"

WinApp* WinApp::GetInstance()
{
	static WinApp instance;

	return &instance;

}

//　ウィンドウプロシャージャ
LRESULT CALLBACK  WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	//　メッセージ固有処理
	switch (msg)
	{
		//　ウィンドウが破棄された
	case WM_DESTROY:
		//　OSに対してアプリが終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	//　メッセージ標準処理
	return DefWindowProc(hwnd, msg, wparam, lparam);

}


void  WinApp::Initialize(const wchar_t* title) {
	WNDCLASS wc{};
	HWND hwnd_;
	const wchar_t* title_=title;
	
	//ウィンドウプロシャージャ
	wc.lpfnWndProc = WindowProc;
	// ウィンドウクラス名
	wc.lpszClassName = L"%s", title_;
	//インスタンドハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	//　カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//ウィンドウクラス登録
	RegisterClass(&wc);
	// クライアント領域サイズ

	//  ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc_ = { 0,0,WinApp::GetInstance()->kClientWidth_ ,WinApp::GetInstance()->kClientHeight_ };
	// クライアント領域を元に実際のサイズにwrcを変更
	AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);
	// ウィンドウ生成
	hwnd_ = CreateWindow(
		wc.lpszClassName,//　クラス名
		title_,                //　タイトルバーの文字
		WS_OVERLAPPEDWINDOW,  //　標準的なウィンドウスタイル
		CW_USEDEFAULT,        //　標準X座標
		CW_USEDEFAULT,        //　標準Y座標
		wrc_.right - wrc_.left, //　横幅
		wrc_.bottom - wrc_.top, //　縦幅ti
		nullptr,              //　親ハンドル
		nullptr,              //　メニューハンドル
		wc.hInstance,         //　インスタンスハンドル
		nullptr               //　オプション
	);
	//ウィンドウを表示
	ShowWindow(hwnd_, SW_SHOW);
	WinApp::GetInstance()->hwnd_ = hwnd_;
	WinApp::GetInstance()->wc_ = wc;
	WinApp::GetInstance()->title_ = title_;
	

}

