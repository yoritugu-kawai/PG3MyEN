#pragma once
#include"../Base/WinApp.h"
#include"../Base/DxCommon.h"
#include"../Base/Function.h"
#include"../externals/DirectXTex/d3dx12.h"
#include"../externals/DirectXTex/DirectXTex.h"
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};
struct TexProeerty {
	D3D12_GPU_DESCRIPTOR_HANDLE SrvHandleGPU;

	ID3D12Resource* Resource;


};
struct Material {

	Vector4 color;
	int32_t enableLighting;
	
};
struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;

};
struct UVMaterial {

	Vector4 color;
	Matrix4x4 uvTransform;

};

class ImageLoading
{
public:
	void Initiluze();
	TexProeerty LoadTexture(const std::string& filePath);
	//void Draw();
	void End();
	DirectX::ScratchImage LoadTextureData(const std::string& filePath);
	ID3D12Resource* CreateTexResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
	void UploadTexData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);
	void ShaderResourceView();

	
private:
	DxCommon dxcommon_;

	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;

};
