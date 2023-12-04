#pragma once

#include"DxCommon.h"
#include"../Shader/PolygonCompileShader.h"

class PolygonPSO
{
public:
	static PolygonPSO* GetInstance();

	//PSO

	static void CreatePSO();
	static void Release();
	static PSO GetPSO() { return PolygonPSO::GetInstance()->pso_; }
private:
	//
	static void ShapePSO();
	HRESULT hr;//

	PSO pso_;
};
