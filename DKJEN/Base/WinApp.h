#pragma once
#include"DxCommon.h"
#include"../Math/Math.h"
#include"../externals/imgui/imgui.h"
#include"../externals/imgui/imgui_impl_dx12.h"
#include"../externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
class WinApp 
{
public :
	static WinApp* GetInstance();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	
	static void Initialize(const wchar_t* title);
	//void Release();
	static const int32_t Width() { return WinApp::GetInstance()-> kClientWidth_; }
	static const int32_t Height() { return WinApp::GetInstance()->kClientHeight_; }
	static HWND GetHwnd() { return WinApp::GetInstance()->hwnd_; }
	static WNDCLASS GetWc() { return WinApp::GetInstance()->wc_; }

private:
	WNDCLASS wc_{};
	HWND hwnd_;
	const wchar_t* title_;
	const int32_t kClientWidth_ = 1280;
	const int32_t kClientHeight_ = 720;
	//ID3D12Debug1* deugController = nullptr;
};


