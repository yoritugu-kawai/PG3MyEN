#pragma once
#include"../Base/DxCommon.h"
#include"../Base/WinApp.h"
class ImguiManager
{
public:
	static ImguiManager* GetInstance();
	static void Initialize();
	static void BeginFrame();
	static void EndFrame();
	static void Release();
private:
	WinApp winApp_;
	DxCommon dxCommon_;
};
