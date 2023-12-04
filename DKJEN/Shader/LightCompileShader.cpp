#include"LightCompileShader.h"

LightCompileShader* LightCompileShader::GetInstance()
{
	static LightCompileShader instance;

	return &instance;
}

IDxcBlob* LightCompileShader::CompileShaderFanc(
	const std::wstring& filePath,
	const wchar_t* profile)
{
	IDxcUtils* dxcUtils = LightCompileShader::GetInstance()->dxcUtils;
	IDxcCompiler3* dxcCompiler = LightCompileShader::GetInstance()->dxcCompiler;
	IDxcIncludeHandler* includeHandler = LightCompileShader::GetInstance()->includeHandler;

	//1.hlslファイルを読む
	Log(ConvertString(std::format(L"Begin CompileShader,path:{},profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr =
		dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	//2.Compileする
	LPCWSTR arguments[] = {
		filePath.c_str(),
		L"-E",L"main",
		L"-T",profile,
		L"-Zi",L"-Qembed_debug",
		L"-Od",
		L"-Zpr",
	};

	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(&shaderSourceBuffer, arguments, _countof(arguments), includeHandler, IID_PPV_ARGS(&shaderResult));
	//コンパイルエラーではなくdxcが起動出来ないなど致命的な状況
	assert(SUCCEEDED(hr));


	//3.警告・エラーが出ていないかを確認する
	//警告・エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		assert(false);
	}


	//4.Compile結果を受け取って返す
	//BLOB・・・BinaryLargeOBject
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePath, profile)));
	//もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;

}


void LightCompileShader::DXC()
{
	IDxcUtils* dxcUtils;
	IDxcCompiler3* dxcCompiler;
	IDxcIncludeHandler* includeHandler;
	// dxcCompilerを初期化
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));


	// 現時点でincludeはしないが、includeに対応するための設定を行っておく
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));
	LightCompileShader::GetInstance()->dxcUtils = dxcUtils;
	LightCompileShader::GetInstance()->dxcCompiler = dxcCompiler;
	LightCompileShader::GetInstance()->includeHandler = includeHandler;
}

void LightCompileShader::ShaderCompile()
{
	ShaderMode shape;

	shape.vertexBlob =
		LightCompileShader::CompileShaderFanc(
			L"LightObject.VS.hlsl",
			L"vs_6_0"
		);
	shape.pixelBlob =
		LightCompileShader::CompileShaderFanc(
			L"LightObject.PS.hlsl",
			L"ps_6_0"
		);


	LightCompileShader::GetInstance()->shaders_.shape = shape;
}

void LightCompileShader::Release()
{
	LightCompileShader::GetInstance()->shaders_.shape.pixelBlob->Release();
	LightCompileShader::GetInstance()->shaders_.shape.vertexBlob->Release();
}

