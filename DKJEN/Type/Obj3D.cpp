#include "Obj3D.h"

void Obj3D::Initialize(TexProeerty  tex)
{
	tex_ = tex;
	modelData = LoadObjFile("resource","axis.obj");


	vetexResource = CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
	materialResource = CreateBufferResource(sizeof(Vector4));
	wvpResource = CreateBufferResource(sizeof(TransformationMatrix));
	


	vertxBufferView.BufferLocation = vetexResource->GetGPUVirtualAddress();
	vertxBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertxBufferView.StrideInBytes = sizeof(VertexData);


	
}

void Obj3D::Draw(Matrix4x4 m)
{

	//
	VertexData* vertexData = nullptr;
	Vector4* materialData = nullptr;
	TransformationMatrix* matrixData = nullptr;
	


	vetexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&matrixData));

	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData)*
		modelData.vertices.size());
	//
	// 

	matrixData->WVP = m;
	matrixData->World = MakeIdentity4x4();
	*materialData = { 1,1,1,1 };


	//
	ID3D12GraphicsCommandList* commandList = DxCommon::GetInstance()->GetCommandList();
	PSOProperty pso_ = TexturePSO::GetInstance()->GetPSO().Texture;
	commandList->SetGraphicsRootSignature(pso_.rootSignature);
	commandList->SetPipelineState(pso_.GraphicsPipelineState);

	commandList->IASetVertexBuffers(0, 1, &vertxBufferView);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

	commandList->SetGraphicsRootDescriptorTable(2, tex_.SrvHandleGPU);
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

	commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
}



ID3D12Resource* Obj3D::CreateBufferResource(size_t sizeInbyte)
{
	ID3D12Device* device = DxCommon::GetInstance()->GetDevice();
	ID3D12Resource* RssultResource;
	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};

	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; //UploadHeapを使う

	//頂点リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};

	//バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeInbyte; //リソースのサイズ。今回はvector4を3頂点分

	//バッファの場合はこれらは1にする決まり
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;

	//バッファの場合はこれにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	HRESULT hr;
	hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&RssultResource));
	assert(SUCCEEDED(hr));

	return RssultResource;
}

ModelData Obj3D::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	//1.
	ModelData modelData;
	std::vector<Vector4>positions;
	std::vector<Vector3>normals;
	std::vector<Vector2>texcoords;
	std::string line;

	//2.
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	//3.
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;
		if (identifier == "v")
		{   //v頂点位置
			Vector4 position = {};
			s >> position.x >> position.y >> position.z;

			position.z *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt")
		{	//vt頂点テクスチャの座標
			Vector2 texcoord = {};
			s >> texcoord.x >> texcoord.y;
			texcoord.y *= -1.0f;

			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn")
		{   //vn頂点法線
			Vector3 normal = {};

			s >> normal.x >> normal.y >> normal.z;
			//normal.z *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3] = {};
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				string vertexDefinition = {};
				s >> vertexDefinition;
				//分解してIndexをGet
				istringstream v(vertexDefinition);
				uint32_t elementIndices[3] = {};
				for (int32_t element = 0; element < 3; ++element) {
					string index;
					getline(v, index, '/');
					elementIndices[element] = stoi(index);
				}

				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position,texcoord,normal };
			}
			//頂点を逆順で登録することで、回り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			//tex
			std::string materialFilename;
			s >> materialFilename;
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	ImageLoading* imgload = new ImageLoading;
	tex_ = imgload->LoadTexture(modelData.material.textureFilePath);

	//4.

	return modelData;
}

MaterialData Obj3D::LoadMaterialTemplateFile(const std::string& directiry, const std::string& filename)
{
	MaterialData materialData;
	std::string line;
	std::ifstream file(directiry + "/" + filename);
	assert(file.is_open());
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			materialData.textureFilePath = directiry + "/" + textureFilename;
		}
	}
	return materialData;
}


void Obj3D::Release()
{
	vetexResource->Release();
	materialResource->Release();
	wvpResource->Release();
	tex_.Resource->Release();

}