#pragma once
#include"WinApp.h"
#include"../Math/Math.h"
#include"../Utilipy/Pch.h"
class DxCommon
{
public:
	static DxCommon* GetInstance();

	
	/// <summary>
	/// コマンド系
	/// </summary>
	static void CreateCommands();

	static void CommandLoad();
	static void Commandkick();
	//ループ
	static void BeginFrame();
	static void EndFrame();

	/// <summary>
	/// スワップチェーン
	/// </summary>
	static void CreateSwapChain();
	static void CreateSwapResce();
	/// <summary>
	/// ヒープ
	/// </summary>
	static void CreateDescriptorHeap();
	static ID3D12DescriptorHeap* CreateDescriptorDesc(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);
	/// <summary>
	/// RTV
	/// </summary>
	static void CreateRTV();
	/// <summary>
	/// エラー
	/// </summary>
	static void DebugInfoQueue();
	static void CreateDxgiFactory();

	//
	static void CreateFeneEvent();
	static void CreateDevice();
	////
	static	ID3D12Resource* CreateDepthStencilTextureRsource(ID3D12Device* device, int32_t width, int32_t height);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="kClientWidth"></param>
	/// <param name="kClientHeight"></param>
	/// <param name="hwnd"></param>
	static void Initialize();


	//解放
	static void Release();

	/// <summary>
	/// 完璧でゲッター
	/// </summary>
	static ID3D12Device* GetDevice() {return DxCommon::GetInstance()->device;}
	static ID3D12GraphicsCommandList* GetCommandList() {return DxCommon::GetInstance()->commandList;}
	static ID3D12DescriptorHeap* GetsrvDescriptorHeap() { return DxCommon::GetInstance()->srvDescriptorHeap; }
	//ここから新しく追加
	static DXGI_SWAP_CHAIN_DESC1 GetswapChainDesc() { return DxCommon::GetInstance()->swapChainDesc; }
	static D3D12_RENDER_TARGET_VIEW_DESC rtvDescGet() { return DxCommon::GetInstance()->rtvDesc; }
private:
	//WinApp* winApp_ = nullptr;

	ComPtr<IDXGIFactory7>m_pDxgiFactory_=nullptr;
	/////
	/// <summary>
	/// ファクトリーの作成
	/// </summary>
	
	HRESULT hr;//
	/// <summary>
	/// アダプターの作成
	/// </summary>
	IDXGIAdapter4* useAdapter;//
	/// <summary>
	/// Deviceの作成
	/// </summary>
	ID3D12Device* device;//
	/// <summary>
	/// エラー警告
	/// </summary>


	ID3D12Debug1* debugController = nullptr;
	/// <summary>
	/// コマンド
	/// </summary>
	ID3D12CommandQueue* commandQueue;//
	ID3D12GraphicsCommandList* commandList;//
	D3D12_RESOURCE_BARRIER barrier{};//
	/// <summary>
	/// スワップチェーン
	/// </summary>
	IDXGISwapChain4* swapChain;//
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};//
	/// <summary>
	/// ヒープ
	/// </summary>
	ID3D12DescriptorHeap* rtvDescriptorHeap;//
	ID3D12DescriptorHeap* srvDescriptorHeap;//
	ID3D12DescriptorHeap* dsvDescriptorHeap;
	/// <summary>
	/// スワップチェーンのリリースを出す
	/// </summary>
	ID3D12Resource* swapChainResources[2];//
	/// <summary>
	/// 
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];//
	/// <summary>
	/// コマンド
	/// </summary>
	ID3D12CommandAllocator* commandAllocator;//
	/// <summary>
	/// デバック
	/// </summary>

	//フェンスイベント
	ID3D12Fence* fence = nullptr;//
	uint64_t fenceValue = 0;//
	HANDLE fenceEvent;//
	//POS
	ID3D12PipelineState* graphicsPipelineState;/*後々の可能性あり*/
	/*---------ルートシグネチャの設定---------*/
	ID3DBlob* signatureBlob;//
	ID3DBlob* errorBlob;//
	ID3D12RootSignature* rootSignature;//
	/*-----シェイダ－コンパイル-------*/
	IDxcBlob* vertexShaderBlob;/*後々の可能性あり*/
	IDxcBlob* pixelShaderBlob;/*後々の可能性あり*/
	//RTV
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	//
	ID3D12Resource* depthStencilResource;
};
