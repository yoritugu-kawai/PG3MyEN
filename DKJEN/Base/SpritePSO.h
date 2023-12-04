#pragma once
#include"DxCommon.h"
#include"../Shader/SpriteCompileShader.h"

class SpritePSO
{
public:
	static SpritePSO* GetInstance();

	//PSO

	static void CreatePSO();
	static void Release();
	static PSO GetPSO() { return SpritePSO::GetInstance()->pso_; }
private:
	//
	static void ShapePSO();
	HRESULT hr;//

	PSO pso_;
};
