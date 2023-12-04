#pragma once
#include"DxCommon.h"
#include"../Shader/LightCompileShader.h"

class LightPSO
{
public:
	static LightPSO* GetInstance();

	//PSO

	static void CreatePSO();
	static void Release();
	static PSO GetPSO() { return LightPSO::GetInstance()->pso_; }
private:
	//
	static void ShapePSO();
	HRESULT hr;//

	PSO pso_;
};
