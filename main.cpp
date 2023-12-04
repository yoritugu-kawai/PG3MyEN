#include"DKJEN/Base/DxCommon.h"
#include"DKJEN/Base/WinApp.h"
#include"DKJEN/Base/ImageLoading.h"

#include"DKJEN/Math/Math.h"
#include"DKJEN/Imgui/imguiManager.h"

#include"DKJEN/Type/Polygon.h"
#include"DKJEN/Type/Sprite.h"
#include"DKJEN/Type/Texture.h"
#include"DKJEN/Type/Sphere.h"
#include"DKJEN/Type/Obj3D.h"
#include"DKJEN/Utilipy/rektyk.h"
#include"DKJEN/Utilipy/Input.h"

#include"DKJEN/Management/PSOCopileManagement.h"
const wchar_t Title[] = { L"ド根性エンジン" };


struct Transfrom4
{
	Vector4 lefe;
	Vector4  top;
	Vector4 right;
	Vector4 color;

};

static D3DResourceLeaker leaker;
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	WinApp::Initialize(Title);
	DxCommon::Initialize();
	ImguiManager::Initialize();
	Input::Initialize();
	
	PSOCopileManagement::Set();
	
	Vector3 axis = Normalize({ 1.0f,1.0f,1.0f });
	float angle = 0.44f;
	

	//　メインループ
	MSG msg{};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		DxCommon::BeginFrame();
		ImguiManager::BeginFrame();
		Input::BeginFlame();

		//　ゲーム処理
		//////


		Matrix4x4 rotateMatrix = MakeRotateAxisAngle(axis, angle);
		ImGui::Begin("rotateMatrix");
		for (int i = 0; i < 4; i++) {
			for (int t = 0; t < 4; t++) {
				ImGui::Text("%.3f", rotateMatrix.m[i][t]);
			}
		}
		
		ImGui::End();
		 
		// 
		//////
		//　ゲーム処理


		ImguiManager::EndFrame();
		DxCommon::EndFrame();

	}

	/*
	*************************
	*********   解放  *******
	*************************
	*/
	

	
	
	PSOCopileManagement::Release();

	ImguiManager::Release();
	DxCommon::Release();


	return 0;
}
