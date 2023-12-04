#include"ImageLoading.h"

static uint32_t LoadCount;

void ImageLoading::Initiluze()
{

	CoInitializeEx(0, COINIT_MULTITHREADED);
	ID3D12Device* device = DxCommon::GetInstance()->GetDevice();
	descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	LoadCount = 0;
}

DirectX::ScratchImage ImageLoading::LoadTextureData(const std::string& filePath)
{

	//テクスチャファイルを読み込んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);

	assert(SUCCEEDED(hr));
	//ミニマップの作成
	DirectX::ScratchImage mipImage{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImage);
	assert(SUCCEEDED(hr));

	//ミニマップ付きのデータを返す
	return mipImage;

}

ID3D12Resource* ImageLoading::CreateTexResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
	//1.metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	//2.利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;


	//3.Resourceを生成する
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource));

	assert(SUCCEEDED(hr));
	return resource;
}

void ImageLoading::UploadTexData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
{
	//Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();

	//全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel)
	{
		//MipMapLevelを指定して各Imagesを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);

		//Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr, //全領域へコピー
			img->pixels, //元データアドレス
			UINT(img->rowPitch), //1ラインサイズ
			UINT(img->slicePitch) //1枚サイズ
		);

		assert(SUCCEEDED(hr));
	}

}

void ImageLoading::ShaderResourceView()
{


}


TexProeerty ImageLoading::LoadTexture(const std::string& filePath)
{
	ID3D12Device* device = DxCommon::GetInstance()->GetDevice();
	ID3D12DescriptorHeap* srvDescriptorHeap = DxCommon::GetInstance()->GetsrvDescriptorHeap();

	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = LoadTextureData(filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();

	ID3D12Resource* textureResource = CreateTexResource(device, metadata);
	UploadTexData(textureResource, mipImages);
	//Textureを読んで転送する2
	//DirectX::ScratchImage mipImages2 = LoadTexture("resource/monsterBall.png");
	//const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	//ID3D12Resource* textureResource2 = CreateTexResource(device, metadata2);
	//UploadTexData(textureResource2, mipImages2);


	//テキストのシェダ－
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	//

	D3D12_GPU_DESCRIPTOR_HANDLE texSrvHandleGPU =
		GetGPUDescriptorHandle(
			srvDescriptorHeap, descriptorSizeSRV, LoadCount);
	D3D12_CPU_DESCRIPTOR_HANDLE texSrvHandleCPU =
		GetCPUDescriptorHandle(
			srvDescriptorHeap, descriptorSizeSRV, LoadCount);

	texSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	texSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	device->CreateShaderResourceView(textureResource, &srvDesc, texSrvHandleCPU);


	//テキストのシェダ－2
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	//srvDesc2.Format = metadata2.format;
	//srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	LoadCount++;
	//

	TexProeerty tex;
	tex.Resource = textureResource;
	tex.SrvHandleGPU = texSrvHandleGPU;

	return tex;
}



void ImageLoading::End()
{
	CoUninitialize();
}
