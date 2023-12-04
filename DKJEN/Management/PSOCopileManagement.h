#pragma once
#include"../Shader/LightCompileShader.h"
#include"../Shader/PolygonCompileShader.h"
#include"../Shader/SpriteCompileShader.h"
#include"../Shader/TextureCompileShader.h"

#include"../Base/LightPSO.h"
#include"../Base/PolygonPSO.h"
#include"../Base/SpritePSO.h"
#include"../Base/TexturePSO.h"

class PSOCopileManagement
{
public:
	static PSOCopileManagement* GetInstance();

	static void Set();
	static void Release();
private:

};
