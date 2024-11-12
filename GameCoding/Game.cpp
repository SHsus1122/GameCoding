#include "pch.h"
#include "Game.h"

Game::Game()
{

}

Game::~Game()
{

}

void Game::Init(HWND hwnd)
{
	_hwnd = hwnd;

	_graphics = new Graphics(hwnd);
	_vertexBuffer = new VertexBuffer(_graphics->GetDevice());
	_indexBuffer = new IndexBuffer(_graphics->GetDevice());
	_inputLayout = new InputLayout(_graphics->GetDevice());

	CreateGeometry();
	CreateVS();
	CreateInputLayout();
	CreatePS();

	CreateResterizerState();
	CreateSamplerState();
	CreateBlendState();

	CreateSRV();
	CreateConstantBuffer();
}

void Game::Update()
{
	//_transformData.offset.x += 0.0003f;
	//_transformData.offset.y += 0.0003f;

	// SRT(Scale - Rotation - Translation) 순서로 생성
	Matrix matScale = Matrix::CreateScale(_localScale / 3);
	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z);
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	// World 행렬로 생성
	Matrix matWorld = matScale * matRotation * matTranslation;	// SRT 순서로 곱
	_transformData.matWrold = matWorld;

	// GPU리소스를 CPU에서 수정 가능하도록 매핑된 데이터를 저장할 구조체를 생성합니다.
	D3D11_MAPPED_SUBRESOURCE subResource;
	ZeroMemory(&subResource, sizeof(subResource));

	// GPU의 상수 버퍼에 새로운 데이터를 쓰기 위해서 CPU에 접근합니다.
	//  - Map을 통해 데이터에 쓰기 전용 모드로 접근하고 memcpy를 통해 매핑된 리소스의 데이터 값을 복사합니다.
	//  - 이후 _transformData(게임 객체 위치나 변형을 담은 구조체)를 통해 렌더링시 사용합니다.
	//  - 마지막으로 Unmap을 통해 _transformData 즉, 객체의 위치 정보를 업데이트 하고 이를 상수 버퍼를 통해 GPU에 전달합니다.
	_graphics->GetDeviceContext()->Map(_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	::memcpy(subResource.pData, &_transformData, sizeof(_transformData));
	_graphics->GetDeviceContext()->Unmap(_constantBuffer.Get(), 0);
}

void Game::Render()
{
	_graphics->RenderBegin();

	// IA - VS - RS - PS - OM
	{
		uint32 stride = sizeof(Vertex);
		uint32 offset = 0;

		auto _deviceContext = _graphics->GetDeviceContext();

		// IA(Input Assembler), 입력 어셈블러
		// Vertex데이터를 수집하고 Index버퍼를 이용해 Vertex의 복제나 중복을 방지하며 정점 데이터를 GPU에 전달하는 단계입니다.
		_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
		_deviceContext->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
		_deviceContext->IASetInputLayout(_inputLayout->GetComPtr().Get());
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// VS(Vertex Shader)
		// 정점 데이터들을 3D공간으로 변환시켜주는 작은 프로그램입니다.(정점의 위치를 계산해 3D 공간에서 2D(화면)공간으로 변환합니다)
		_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
		_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());

		// RS(Rasterize)
		// Vertex Shader가 넘겨준 부분에 해당하는 모든 픽셀을 판별(변환)하는 단계입니다.
		_deviceContext->RSSetState(_rasterizerState.Get());

		// PS(Pixel Shader)
		// 그리고자 하는 오브젝트(모델, 리소스)에 색상을 입힐 때 사용하는 작은 프로그램입니다.
		// 보여지는 모든 픽셀들에 대해 GPU에서 연산이 됩니다.(색을 입히는 과정이 아니라 계산하는(최종 출력 결정) 아닙니다)
		_deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);
		_deviceContext->PSSetShaderResources(0, 1, _shaderResourceView.GetAddressOf());
		_deviceContext->PSSetSamplers(0, 1, _samplerState.GetAddressOf());

		// OM(Output Merge)
		// 렌더링 파이프라인의 마지막 단계로 RTV에 그리기 전에 최종적으로 병합하는 단계입니다.(모든 픽셀 쉐이더를 화면에 그리는 단계)
		_deviceContext->OMSetBlendState(_blendState.Get(), nullptr, 0xFFFFFFFF);

		//_deviceContext->Draw(_vertices.size(), 0);
		_deviceContext->DrawIndexed(_indices.size(), 0, 0);
	}

	_graphics->RenderEnd();
}


// 기하 데이터를 생성해 GPU에 전달합니다.(정점 데이터 정의 및 Vertex Buffer 생성)
// 삼각형을 그린다 하면 화면에 그려질 삼각형을 정의하고 GPU에서 사용할 수 있도록 합니다.
void Game::CreateGeometry()
{
	// VertextData 정점 데이터 생성
	{
		_vertices.resize(4);

		_vertices[0].position = Vec3(-0.5f, -0.5f, 0.f);
		_vertices[0].uv = Vec2(0.f, 1.f);
		//_vertices[0].color = Color(1.f, 0.f, 0.f, 1.f);

		_vertices[1].position = Vec3(-0.5f, 0.5f, 0.f);
		_vertices[1].uv = Vec2(0.f, 0.f);
		//_vertices[1].color = Color(1.f, 0.f, 0.f, 1.f);

		_vertices[2].position = Vec3(0.5f, -0.5f, 0.f);
		_vertices[2].uv = Vec2(1.f, 1.f);
		//_vertices[2].color = Color(1.f, 0.f, 0.f, 1.f);

		_vertices[3].position = Vec3(0.5f, 0.5f, 0.f);
		_vertices[3].uv = Vec2(1.f, 0.f);
		//_vertices[3].color = Color(1.f, 0.f, 0.f, 1.f);
	}

	// VertexBuffer (GPU에서 이뤄지는 작업)
	{
		_vertexBuffer->Create(_vertices);
	}

	// index 리스트(버텍스의 위치를 참조해서 구성하며 이를 통해 중복 방지로 메모리를 절약합니다.)
	{
		// 시계 방향
		_indices = { 0, 1, 2, 2, 1, 3 };
	}

	// IndexBuffer
	{
		_indexBuffer->Create(_indices);
	}
}

// 정점 버퍼에서 데이터를 가져와 CreateInputLayout에서 정의한 시맨틱을 통해 쉐이더로 전달(전달하기 위한 레이아웃 정의)
// 정점 데이터가 GPU로 전달될 때, 쉐이더에서 데이터를 어떻게 해석할지를 결정하는 역할입니다.
void Game::CreateInputLayout()
{
	// 입력 버퍼 데이터를 설명하는 입력 레이아웃 객체 생성
	vector<D3D11_INPUT_ELEMENT_DESC> layout
	{
		// POSITION은 변수명, COLOR의 12는 바이트 위치(직접 만든 struct 참고)
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	_inputLayout->Create(layout, _vsBlob);
}

// 버퍼 데이터 생성
void Game::CreateVS()
{
	LoadShaderFromFile(L"Default.hlsl", "VS", "vs_5_0", _vsBlob);
	HRESULT hr = _graphics->GetDevice()->CreateVertexShader(_vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), nullptr, _vertexShader.GetAddressOf());
	CHECK(hr);
}

// 픽셀 데이터 생성
void Game::CreatePS()
{
	LoadShaderFromFile(L"Default.hlsl", "PS", "ps_5_0", _psBlob);
	HRESULT hr = _graphics->GetDevice()->CreatePixelShader(_psBlob->GetBufferPointer(), _psBlob->GetBufferSize(), nullptr, _pixelShader.GetAddressOf());
	CHECK(hr);
}

// Resterizer(레스터라이저)
// 이는 쉽게 말해 3D장면을 2D이미지로 변환하는 과정을 말합니다.
// 3D객체의 기하학적 데이터를 화면의 픽셀 그리드로 변환하는 파이프라인에서 화면에 그림을 실제로 출력하기 위한 마지막 주요 단계입니다.
// 결과적으로 레스터화된 이미지는 픽셀 그리드로 이루어진 화면에 실제로 표현할 수 있는 형태의 2D 이미지가 됩니다.
void Game::CreateResterizerState()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	// FillMode : 삼각형은 렌더링할 때 사용할 채우기 모드입니다. Solid는 채우는 옵션입니다.
	// CullMode : 특정 방향을 향한 삼각형을 그리지 않습니다. Back은 뒤를 향한 삼각형을 그리지 않습니다. 
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = false;	// 시계 방향(삼각형 기준 좌표 0, 1, 2 로 그린다고 하면)

	HRESULT hr = _graphics->GetDevice()->CreateRasterizerState(&desc, _rasterizerState.GetAddressOf());
	CHECK(hr);
}

// 샘플링은 주어진 UV 좌표에 해당하는 텍스처 픽셀(텍셀)을 읽어와서 색상 데이터를 제공하는 과정입니다.
// Sampler State는 이 샘플링 과정에서 텍스처 좌표가 범위를 벗어났을 때 처리 방식, 텍셀 간의 보간(interpolation) 방식, 필터링 등을 정의합니다.
void Game::CreateSamplerState()
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;	// 범위를 벗어날 경우 텍스처 처리 방식
	desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.BorderColor[0] = 1;	// 범위를 벗어날 경우 사용할 색상
	desc.BorderColor[1] = 0;
	desc.BorderColor[2] = 0;
	desc.BorderColor[3] = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;	// 텍스처 샘플링 비교 함수(Always는 비교X)
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// 텍셀 간의 필터링(보간)방식
	desc.MaxAnisotropy = 16;
	desc.MaxLOD = FLT_MAX;
	desc.MinLOD = FLT_MIN;
	desc.MipLODBias = 0.0f;

	_graphics->GetDevice()->CreateSamplerState(&desc, _samplerState.GetAddressOf());
}

// Blending은 여러 픽셀의 색상을 알파값을 기준으로 합성해 최종 픽셀 색상을 결정하는 과정입니다.
// 이 과정은 현재 픽셀과 대상 픽셀을 특정 연산을 통해 결합하는 방식으로 이루어집니다.
// CreateBlendState는 이 블렌딩을 정의하는 블렌드 상태를 설정해 어떤 방식으로 색상을 합성할지 결정합니다.
void Game::CreateBlendState()
{
	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	_graphics->GetDevice()->CreateBlendState(&desc, _blendState.GetAddressOf());
}

// Shader Resource View
// 쉐이더에 리소스를 제공하기 위해 사용되는 인터페이스입니다. 즉, 리소스를 파이프라인에 연결해줍니다.
// 쉐이더 단계에서 자원을 읽을 수 있게 해줍니다.(여기서는 외부 소스파일 png를 불러와서 사용하게 합니다)
void Game::CreateSRV()
{
	DirectX::TexMetadata md;
	DirectX::ScratchImage img;
	HRESULT hr = ::LoadFromWICFile(L"Golem.png", WIC_FLAGS_NONE, &md, img);
	CHECK(hr);

	// 이미지를 불러온 다음 리소스뷰 생성
	hr = ::CreateShaderResourceView(_graphics->GetDevice().Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourceView.GetAddressOf());
	CHECK(hr);
}

// 상수 버퍼 생성(ConstantBuffer란 장면의 물체마다 달라지는 상수 데이터를 담기 위한 저장공간을 말합니다.)
// 이를 통해 움직임이 있을 때, 매번 버텍스를 새로이 그리는게 아니라 한 번 그린 정보를 바탕으로 상수 버퍼만을 추가해 움직임을 구현합니다.(리소스 절약)
void Game::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC;				// CPU_Write + GPU_Read 방식
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// 상수 버퍼
	desc.ByteWidth = sizeof(TransformData);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = _graphics->GetDevice()->CreateBuffer(&desc, nullptr, _constantBuffer.GetAddressOf());
	CHECK(hr);
}

// 쉐이더 파일(HLSL)를 로드하고 컴파일 합니다.
void Game::LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob)
{
	// 컴파일 플래그, 디버그 용도 및 최적화 건너뛰는 옵션
	const uint32 compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	// D3DCompileFromFile : 쉐이더를 컴파일하고 결과를 "blob"에 저장합니다.
	HRESULT hr = ::D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		name.c_str(),
		version.c_str(),
		compileFlag,
		0,
		blob.GetAddressOf(),
		nullptr);

	CHECK(hr);
}
