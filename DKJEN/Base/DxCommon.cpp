#include"DxCommon.h"

DxCommon* DxCommon::GetInstance()
{
	static DxCommon instance;

	return &instance;
}

/************************************************/
/****************   コマンド組   ****************/
/************************************************/
void DxCommon::CreateCommands()
{
	// コマンドキュー作成

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	ID3D12Device* device = DxCommon::GetInstance()->device;//
	HRESULT hr;//
	ID3D12CommandQueue* commandQueue;//
	ID3D12GraphicsCommandList* commandList;//
	ID3D12CommandAllocator* commandAllocator;//
	//
	hr = device->CreateCommandQueue(&commandQueueDesc,
		IID_PPV_ARGS(&commandQueue));
	// コマンドキュー作成がうまくいかなかった
	assert(SUCCEEDED(hr));
	// CommandList作成

	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(hr));

	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr,
		IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(hr));
	DxCommon::GetInstance()->device = device;
	DxCommon::GetInstance()->commandQueue = commandQueue;
	DxCommon::GetInstance()->commandList = commandList;
	DxCommon::GetInstance()->commandAllocator = commandAllocator;
}


void DxCommon::CommandLoad()
{
	IDXGISwapChain4* swapChain = DxCommon::GetInstance()->swapChain;
	D3D12_RESOURCE_BARRIER barrier{};//
	ID3D12Resource* swapChainResources[2];//
	swapChainResources[0] = DxCommon::GetInstance()->swapChainResources[0];
	swapChainResources[1] = DxCommon::GetInstance()->swapChainResources[1];
	ID3D12DescriptorHeap* rtvDescriptorHeap = DxCommon::GetInstance()->rtvDescriptorHeap;//
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];//
	rtvHandles[0] = DxCommon::GetInstance()->rtvHandles[0];
	rtvHandles[1] = DxCommon::GetInstance()->rtvHandles[1];
	ID3D12GraphicsCommandList* commandList = DxCommon::GetInstance()->commandList;//
	ID3D12DescriptorHeap* dsvDescriptorHeap = DxCommon::GetInstance()->dsvDescriptorHeap;

	///
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = swapChainResources[backBufferIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList->ResourceBarrier(1, &barrier);

	commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

	///
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	//
	DxCommon::GetInstance()->dsvDescriptorHeap = dsvDescriptorHeap;
	DxCommon::GetInstance()->swapChain = swapChain;
	DxCommon::GetInstance()->barrier = barrier;
	DxCommon::GetInstance()->swapChainResources[0] = swapChainResources[0];
	DxCommon::GetInstance()->swapChainResources[1] = swapChainResources[1];
	DxCommon::GetInstance()->rtvDescriptorHeap = rtvDescriptorHeap;
	DxCommon::GetInstance()->rtvHandles[0] = rtvHandles[0];
	DxCommon::GetInstance()->rtvHandles[1] = rtvHandles[1];
	DxCommon::GetInstance()->commandList = commandList;
}

void DxCommon::Commandkick()
{
	IDXGISwapChain4* swapChain = DxCommon::GetInstance()->swapChain;
	D3D12_RESOURCE_BARRIER barrier = DxCommon::GetInstance()->barrier;
	ID3D12GraphicsCommandList* commandList = DxCommon::GetInstance()->commandList;//
	ID3D12CommandQueue* commandQueue = DxCommon::GetInstance()->commandQueue;//
	ID3D12CommandAllocator* commandAllocator = DxCommon::GetInstance()->commandAllocator;//
	ID3D12Fence* fence = DxCommon::GetInstance()->fence;//
	uint64_t fenceValue = DxCommon::GetInstance()->fenceValue;//
	HANDLE fenceEvent = DxCommon::GetInstance()->fenceEvent;//
	HRESULT hr;//
	////
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	commandList->ResourceBarrier(1, &barrier);
	//
	hr = commandList->Close();
	assert(SUCCEEDED(hr));
	//コマンドキック
	ID3D12CommandList* commandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(1, commandLists);
	swapChain->Present(0, 1);
	//シグナル
	fenceValue++;
	commandQueue->Signal(fence, fenceValue);
	if (fence->GetCompletedValue() < fenceValue) {
		fence->SetEventOnCompletion(fenceValue, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
	}
	swapChain->Present(1, 0);

	///
	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));

	DxCommon::GetInstance()->swapChain = swapChain;
	DxCommon::GetInstance()->barrier = barrier;
	DxCommon::GetInstance()->commandList = commandList;//
	DxCommon::GetInstance()->commandQueue = commandQueue;//
	DxCommon::GetInstance()->commandAllocator = commandAllocator;//
	DxCommon::GetInstance()->fence = fence;//
	DxCommon::GetInstance()->fenceValue = fenceValue;//
	DxCommon::GetInstance()->fenceEvent = fenceEvent;//

}

void DxCommon::BeginFrame()
{
	CommandLoad();

	D3D12_VIEWPORT viewport{};
	ID3D12GraphicsCommandList* commandList = DxCommon::GetInstance()->commandList;//

	//クライアント領域のサイズを一緒にして画面全体に表示
	viewport.Width = float(WinApp::GetInstance()->Width());
	viewport.Height = float(WinApp::GetInstance()->Height());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;


	//シザー矩形
	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.right = WinApp::GetInstance()->Width();
	scissorRect.top = 0;
	scissorRect.bottom = WinApp::GetInstance()->Height();

	//コマンドを積む


	commandList->RSSetViewports(1, &viewport); //
	commandList->RSSetScissorRects(1, &scissorRect);
	//



}
void DxCommon::EndFrame()
{
	Commandkick();

}

/************************************************/
/*************   スワップチェーン　**************/
/************************************************/
void DxCommon::CreateSwapChain()
{
	IDXGISwapChain4* swapChain = DxCommon::GetInstance()->swapChain;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = DxCommon::GetInstance()->swapChainDesc;
	ID3D12CommandQueue* commandQueue = DxCommon::GetInstance()->commandQueue;//
	IDXGIFactory7* dxgiFactory = DxCommon::GetInstance()->m_pDxgiFactory_.Get();//
	///
	//   スワップチェーン作成


	swapChainDesc.Width = WinApp::GetInstance()->Width();
	swapChainDesc.Height = WinApp::GetInstance()->Height();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue, WinApp::GetInstance()->GetHwnd(),
		&swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
	assert(SUCCEEDED(hr));
	DxCommon::GetInstance()->commandQueue = commandQueue;//
	DxCommon::GetInstance()->m_pDxgiFactory_ = dxgiFactory;//
	DxCommon::GetInstance()->swapChain = swapChain;
	DxCommon::GetInstance()->swapChainDesc = swapChainDesc;
}
void DxCommon::CreateSwapResce()
{
	IDXGISwapChain4* swapChain = DxCommon::GetInstance()->swapChain;
	ID3D12Resource* swapChainResources[2];//
	swapChainResources[0] = DxCommon::GetInstance()->swapChainResources[0];
	swapChainResources[1] = DxCommon::GetInstance()->swapChainResources[1];
	///
	//スワップチェーンリソースを引っ張る

	HRESULT hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));
	DxCommon::GetInstance()->swapChain = swapChain;
	DxCommon::GetInstance()->swapChainResources[0] = swapChainResources[0];
	DxCommon::GetInstance()->swapChainResources[1] = swapChainResources[1];
}

/************************************************/
/********************   ヒープ　*****************/
/************************************************/
void DxCommon::CreateDescriptorHeap()
{
	ID3D12DescriptorHeap* rtvDescriptorHeap;
	ID3D12DescriptorHeap* srvDescriptorHeap;
	ID3D12DescriptorHeap* dsvDescriptorHeap;
	ID3D12Resource* depthStencilResource;
	ID3D12Device* device = DxCommon::GetInstance()->device;
	///
	//ディスクトップヒープ作成
	rtvDescriptorHeap = CreateDescriptorDesc(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	srvDescriptorHeap = CreateDescriptorDesc(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	/*D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc.NumDescriptors = 2;
	HRESULT hr = device->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap));
	assert(SUCCEEDED(hr));*/
	CreateSwapResce();
	//DSV
     depthStencilResource = CreateDepthStencilTextureRsource(device, WinApp::GetInstance()->Width(), WinApp::GetInstance()->Height());
	dsvDescriptorHeap = CreateDescriptorDesc(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	device->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());


	//引き渡し
	DxCommon::GetInstance()->depthStencilResource = depthStencilResource;
	DxCommon::GetInstance()->device = device;
	DxCommon::GetInstance()->rtvDescriptorHeap = rtvDescriptorHeap;
	DxCommon::GetInstance()->srvDescriptorHeap = srvDescriptorHeap;
	DxCommon::GetInstance()->dsvDescriptorHeap = dsvDescriptorHeap;
	CreateRTV();
}

ID3D12DescriptorHeap* DxCommon::CreateDescriptorDesc(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));

	assert(SUCCEEDED(hr));

	return descriptorHeap;
}

/************************************************/
/********************   RTV　 *******************/
/************************************************/

void DxCommon::CreateRTV()
{
	ID3D12DescriptorHeap* rtvDescriptorHeap = DxCommon::GetInstance()->rtvDescriptorHeap;
	ID3D12Resource* swapChainResources[2];//
	swapChainResources[0] = DxCommon::GetInstance()->swapChainResources[0];
	swapChainResources[1] = DxCommon::GetInstance()->swapChainResources[1];
	ID3D12Device* device = DxCommon::GetInstance()->device;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];//
	rtvHandles[0] = DxCommon::GetInstance()->rtvHandles[0];
	rtvHandles[1] = DxCommon::GetInstance()->rtvHandles[1];
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = DxCommon::GetInstance()->rtvDesc;
	////
	// RTVです

	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvStarHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandles[0] = rtvStarHandle;
	device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);

	rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);
	//



	DxCommon::GetInstance()->rtvDesc = rtvDesc;
	DxCommon::GetInstance()->device = device;
	DxCommon::GetInstance()->rtvDescriptorHeap = rtvDescriptorHeap;
	DxCommon::GetInstance()->swapChainResources[0] = swapChainResources[0];
	DxCommon::GetInstance()->swapChainResources[1] = swapChainResources[1];
	DxCommon::GetInstance()->rtvHandles[0] = rtvHandles[0];
	DxCommon::GetInstance()->rtvHandles[1] = rtvHandles[1];

}
/************************************************/
/****************   　エラー　   ****************/
/************************************************/

void DxCommon::DebugInfoQueue() {

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;//
	ID3D12Device* device = DxCommon::GetInstance()->device;
	///
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		D3D12_MESSAGE_ID denyIds[]{
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;

		infoQueue->PushStorageFilter(&filter);
		infoQueue->Release();

	}
	DxCommon::GetInstance()->device = device;
#endif // _DEBUG

}
void DxCommon::CreateDxgiFactory()
{
	IDXGIFactory7* dxgiFactory;//
	//GXGFactoryの生成

	//HRESULTはWindows系のエラーコードあり
	//関数が成功したかどうかをSUCCEDEDマクロで判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));

	//初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));

	//アダプタ決定
	IDXGIAdapter4* useAdapter;
	// 良い順にアダプタ
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter))
		!= DXGI_ERROR_NOT_FOUND; ++i) {

		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));
		//ソフトウェアアダプタでなければ採用！
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用したアダプタの情報をログに出力
			Log(ConvertString(std::format(L"Use Adapater:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;
	}
	assert(useAdapter != nullptr);
	DxCommon::GetInstance()->m_pDxgiFactory_ = dxgiFactory;
	DxCommon::GetInstance()->useAdapter = useAdapter;


}
/************************************************/
/***********   　よくわからない　   *************/
/************************************************/
void DxCommon::CreateFeneEvent()
{
	ID3D12Fence* fence = nullptr;//
	uint64_t fenceValue = 0;//
	HANDLE fenceEvent;//
	ID3D12Device* device = DxCommon::GetInstance()->device;
	///
	HRESULT hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);
	//
	DxCommon::GetInstance()->device = device;
	DxCommon::GetInstance()->fence = fence;
	DxCommon::GetInstance()->fenceValue = fenceValue;
	DxCommon::GetInstance()->fenceEvent = fenceEvent;

}
void DxCommon::CreateDevice()
{
	IDXGIAdapter4* useAdapter = DxCommon::GetInstance()->useAdapter;
	ID3D12Device* device = DxCommon::GetInstance()->device;
	///
	//D3D12Deviceの作成

	D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_12_2,	D3D_FEATURE_LEVEL_12_1,	D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		HRESULT hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
		if (SUCCEEDED(hr)) {
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	assert(device != nullptr);
	Log("Complete Create D3D12Device!!!\n");
	////
	DxCommon::GetInstance()->device = device;
	DxCommon::GetInstance()->useAdapter = useAdapter;
}
/***************************************/
/****************          *************/
/***************************************/
ID3D12Resource* DxCommon::CreateDepthStencilTextureRsource(ID3D12Device* device, int32_t width, int32_t height)
{
	


	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//
	D3D12_HEAP_PROPERTIES heapProperties{ };
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	//
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット。Resourceと合わせる


	// Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Hepaの特殊な設定。特になし
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
		&depthClearValue, // Clear最適値
		IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ

	assert(SUCCEEDED(hr));
	
	return resource;
}
/************************************************/
/****************   　初期化　   ****************/
/************************************************/
void DxCommon::Initialize() {

	/* ----- デバッグレイヤー -----*/
#ifdef _DEBUG

	ID3D12Debug1* debugController = nullptr;

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバッグレイヤーを有効にする
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
	DxCommon::GetInstance()->debugController = debugController;
#endif // _DEBUG
	CreateDxgiFactory();
	CreateDevice();
	DebugInfoQueue();
	CreateCommands();

	CreateSwapChain();
	CreateDescriptorHeap();
	CreateFeneEvent();
}



void DxCommon::Release() {

	CloseHandle(DxCommon::GetInstance()->fenceEvent);
	DxCommon::GetInstance()->fence->Release();
	DxCommon::GetInstance()->rtvDescriptorHeap->Release();
	DxCommon::GetInstance()->srvDescriptorHeap->Release();
	DxCommon::GetInstance()->dsvDescriptorHeap->Release();//新しく追加

	DxCommon::GetInstance()->swapChainResources[0]->Release();
	DxCommon::GetInstance()->swapChainResources[1]->Release();
	DxCommon::GetInstance()->swapChain->Release();

	DxCommon::GetInstance()->commandList->Release();
	DxCommon::GetInstance()->commandAllocator->Release();
	DxCommon::GetInstance()->commandQueue->Release();
	DxCommon::GetInstance()->device->Release();
	DxCommon::GetInstance()->useAdapter->Release();

	//DxCommon::GetInstance()->m_pDxgiFactory_->Release();

	DxCommon::GetInstance()->depthStencilResource->Release();
#ifdef _DEBUG
	DxCommon::GetInstance()->debugController->Release();
#endif // _DEBUG

	CloseWindow(WinApp::GetInstance()->GetHwnd());

	//リソースリークチェック

	
}