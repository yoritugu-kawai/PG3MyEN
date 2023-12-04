#pragma once
#include<Windows.h>
#include <cstdint>
#include<string>
#include<format>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>
#include<dxgidebug.h>
#include<dxcapi.h>
#include <cassert>
#include <cmath>
#include <numbers>

#include"../Base/Log.h"
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxcompiler.lib")

struct PSOProperty
{
	ID3D12PipelineState* GraphicsPipelineState = nullptr;
	ID3D12RootSignature* rootSignature = nullptr;
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

};
struct PSO
{
	PSOProperty Texture;
	PSOProperty polygon;

};
struct  ShaderMode
{
	IDxcBlob* vertexBlob;
	IDxcBlob* pixelBlob;
};
struct Shaders {
	ShaderMode shape;
};

struct Vector2 {
	float x;
	float y;
};
struct  Vector3
{
	float x;
	float y;
	float z;

};
struct Vector4
{
	float x;
	float y;
	float z;
	float w;
};
struct   Matrix4x4 {
	float m[4][4];
};

struct  Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
	Matrix4x4 matWorld;
};

struct ImGguiTransfrom
{
	Matrix4x4 matrix;
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
	Vector4 color;
};

/// <summary>
/// 数学
/// </summary>
/// <returns></returns>
Matrix4x4 MakeIdentity4x4();
Matrix4x4 MakeScaleMatrix(const Vector3 scale);
Matrix4x4 MakeRotateXMatrix(float radian);
Matrix4x4 MakeRotateYMatrix(float radian);
Matrix4x4 MakeRotateZMatrix(float radian);
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 MakeRotateXYZMatrix(float radianX, float radianY, float radianZ);
Matrix4x4 MakeTranslateMatrix(const Vector3 translate);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
Matrix4x4 MakePerspectiveFovMatrix(
	float fovY, float aspecrRatio, float nearClip, float farClip);
Matrix4x4 Inverse(const Matrix4x4& m);
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float neaCrlip, float farClip);
float Length(const Vector3& v);
Vector3 Normalize(const Vector3& v);
Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);


