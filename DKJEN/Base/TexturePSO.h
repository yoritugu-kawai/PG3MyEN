#pragma once
#include"DxCommon.h"
#include"../Shader/TextureCompileShader.h"

class TexturePSO
{
public:
	static TexturePSO* GetInstance();
	
	//PSO
	
	static void CreatePSO();
	static void Release();
	static PSO GetPSO() {return TexturePSO::GetInstance()->pso_;}
private:
	//
	static void ShapePSO();
	HRESULT hr;//
	
	PSO pso_;
};
