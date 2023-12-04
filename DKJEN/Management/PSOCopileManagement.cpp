#include"PSOCopileManagement.h"

//PSOCopileManagement PSOCopileManagement::GetInstance()
//{
//	static PSOCopileManagement instance;
//
//	return &instance;
//}

PSOCopileManagement* PSOCopileManagement::GetInstance()
{
	static PSOCopileManagement instance;
	return &instance;
}

void PSOCopileManagement::Set()
{
	TextureCompileShader::DXC();
	PolygonCompileShader::DXC();
	SpriteCompileShader::DXC();
	LightCompileShader::DXC();

	TextureCompileShader::ShaderCompile();
	PolygonCompileShader::ShaderCompile();
	SpriteCompileShader::ShaderCompile();
	LightCompileShader::ShaderCompile();

	TexturePSO::CreatePSO();
	PolygonPSO::CreatePSO();
	SpritePSO::CreatePSO();
	LightPSO::CreatePSO();
}

void PSOCopileManagement::Release()
{
	TextureCompileShader::Release();
	PolygonCompileShader::Release();
	SpriteCompileShader::Release();
	LightCompileShader::Release();
	TexturePSO::Release();
	PolygonPSO::Release();
	SpritePSO::Release();
	LightPSO::Release();

}
