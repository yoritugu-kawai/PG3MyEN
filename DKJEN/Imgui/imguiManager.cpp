#include"../Imgui/imguiManager.h"

ImguiManager* ImguiManager::GetInstance()
{
	static ImguiManager instance;

	return &instance;
}


void  ImguiManager::Initialize() {
	ID3D12Device* device = DxCommon::GetInstance()->GetDevice();
	ID3D12DescriptorHeap* srvDescriptorHeap = DxCommon::GetInstance()->GetsrvDescriptorHeap();
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = DxCommon::GetInstance()->GetswapChainDesc();
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = DxCommon::GetInstance()->rtvDescGet();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(device,
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		srvDescriptorHeap,
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
}

void ImguiManager::BeginFrame()
{
	ID3D12DescriptorHeap* srvDescriptorHeap = DxCommon::GetInstance()->GetsrvDescriptorHeap();
	ID3D12GraphicsCommandList* commandList = DxCommon::GetInstance()->GetCommandList();
	
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap };
	commandList->SetDescriptorHeaps(1, descriptorHeaps);
}

void ImguiManager::EndFrame()
{
	ID3D12GraphicsCommandList* commandList = DxCommon::GetInstance()->GetCommandList();
	//ImGui::ShowDemoWindow();
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

void ImguiManager::Release()
{
	ImGui_ImplDX12_Shutdown();
}
