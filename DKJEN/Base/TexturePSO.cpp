#include "TexturePSO.h"
TexturePSO* TexturePSO::GetInstance()
{
	static TexturePSO instance;

	return &instance;
}
void TexturePSO::CreatePSO()
{
	ShapePSO();
}

void TexturePSO::Release()
{
	if (TexturePSO::GetInstance()->pso_.Texture.errorBlob)
	{
		TexturePSO::GetInstance()->pso_.Texture.errorBlob->Release();
	}
	TexturePSO::GetInstance()->pso_.Texture.GraphicsPipelineState->Release();
	TexturePSO::GetInstance()->pso_.Texture.rootSignature->Release();
	TexturePSO::GetInstance()->pso_.Texture.signatureBlob->Release();
}

void TexturePSO::ShapePSO()
{

	ID3D12Device* device = DxCommon::GetInstance()->GetDevice();
	//ID3D12GraphicsCommandList* commandList = DxCommon::GetInstance()->GetCommandList();

	PSOProperty ShapePSO;

	ShaderMode shader = TextureCompileShader::GetInstance()->GetShaders().shape;

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//Material設定
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	//VertexのTransform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;
	////
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	//
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
	//
	D3D12_STATIC_SAMPLER_DESC staicSamplers[1] = {};
	staicSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staicSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staicSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staicSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staicSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staicSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staicSamplers[0].ShaderRegister = 0;
	staicSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//
	descriptionRootSignature.pStaticSamplers = staicSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staicSamplers);
	////
	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);



	//シリアライズしてバイナリにする

	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &ShapePSO.signatureBlob, &ShapePSO.errorBlob);
	if (FAILED(hr))
	{
		Log(reinterpret_cast<char*>(ShapePSO.errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリを元に生成

	hr = device->CreateRootSignature(
		0,
		ShapePSO.signatureBlob->GetBufferPointer(),
		ShapePSO.signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&ShapePSO.rootSignature)
	);
	assert(SUCCEEDED(hr));




	//InputLayoutの設定
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	//
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	//BlendStateの設定を行う
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;


	//RasterrizerStateぼ設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	//裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//ステイト追加
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//

	////PSOの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = ShapePSO.rootSignature; //RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; //InputLayout


	///
	graphicsPipelineStateDesc.VS = { shader.vertexBlob->GetBufferPointer(),
	shader.vertexBlob->GetBufferSize() }; //VertexShader

	//
	graphicsPipelineStateDesc.PS = { shader.pixelBlob->GetBufferPointer(),
	shader.pixelBlob->GetBufferSize() }; //PixeShader

	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; //RasterizerState
	///
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&ShapePSO.GraphicsPipelineState));
	assert(SUCCEEDED(hr));

	TexturePSO::GetInstance()->pso_.Texture = ShapePSO;


}
