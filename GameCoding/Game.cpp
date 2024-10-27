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
	_width = GWinSizeX;
	_height = GWinSizeY;

	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
	SetViewport();

	CreateGeometry();
	CreateVS();
	CreateInputLayout();
	CreatePS();

	CreateSRV();
	CreateConstantBuffer();
}

void Game::Update()
{
	_transformData.offset.x += 0.0003f;
	_transformData.offset.y += 0.0003f;

	// GPU리소스를 CPU에서 수정 가능하도록 매핑된 데이터를 저장할 구조체를 생성합니다.
	D3D11_MAPPED_SUBRESOURCE subResource;
	ZeroMemory(&subResource, sizeof(subResource));

	// GPU의 상수 버퍼에 새로운 데이터를 쓰기 위해서 CPU에 접근합니다.
	//  - Map을 통해 데이터에 쓰기 전용 모드로 접근하고 memcpy를 통해 매핑된 리소스의 데이터 값을 복사합니다.
	//  - 이후 _transformData(게임 객체 위치나 변형을 담은 구조체)를 통해 렌더링시 사용합니다.
	//  - 마지막으로 Unmap을 통해 _transformData 즉, 객체의 위치 정보를 업데이트 하고 이를 상수 버퍼를 통해 GPU에 전달합니다.
	_deviceContext->Map(_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	::memcpy(subResource.pData, &_transformData, sizeof(_transformData));
	_deviceContext->Unmap(_constantBuffer.Get(), 0);
}

void Game::Render()
{
	RenderBegin();

	// IA - VS - RS - PS - OM
	{
		uint32 stride = sizeof(Vertex);
		uint32 offset = 0;

		// IA(Input Assembler), 입력 어셈블러
		// Vertex데이터를 수집하고 Index버퍼를 이용해 Vertex의 복제나 중복을 방지하며 정점 데이터를 GPU에 전달하는 단계입니다.
		_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
		_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		_deviceContext->IASetInputLayout(_inputLayout.Get());
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// VS(Vertex Shader)
		// 정점 데이터들을 3D공간으로 변환시켜주는 작은 프로그램입니다.(정점의 위치를 계산해 3D 공간에서 2D(화면)공간으로 변환합니다)
		_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
		_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());

		// RS(Rasterize)
		// Vertex Shader가 넘겨준 부분에 해당하는 모든 픽셀을 판별(변환)하는 단계입니다.

		// PS(Pixel Shader)
		// 그리고자 하는 오브젝트(모델, 리소스)에 색상을 입힐 때 사용하는 작은 프로그램입니다.
		// 보여지는 모든 픽셀들에 대해 GPU에서 연산이 됩니다.(색을 입히는 과정이 아니라 계산하는(최종 출력 결정) 아닙니다)
		_deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);
		_deviceContext->PSSetShaderResources(0, 1, _shaderResourceView.GetAddressOf());

		// OM(Output Merge)
		// 렌더링 파이프라인의 마지막 단계로 RTV에 그리기 전에 최종적으로 병합하는 단계입니다.(모든 픽셀 쉐이더를 화면에 그리는 단계)
		//_deviceContext->Draw(_vertices.size(), 0);
		_deviceContext->DrawIndexed(_indices.size(), 0, 0);
	}

	RenderEnd();
}

void Game::RenderBegin()
{
	// 화면을 그려주고, 비워주고, 뷰포트에 설정
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);
	_deviceContext->RSSetViewports(1, &_viewPort);
}

void Game::RenderEnd()
{
	// Present : 다 그린 전면 버퍼를 복사 즉, 출력하는 명령어
	HRESULT hr = _swapChain->Present(1, 0);
	CHECK(hr);
}

// 그래픽 장치 초기화 및 화면에 출력을 위한 스왑체인 생성(화면 버퍼 만들기 작업)
//	- Device	: GPU와 소통, 그래픽 리소스 생성 및 파이프라인의 관리
//	- SwapChain : 전/후면 버퍼 관리 및 렌더링된 이미지를 화면에 출력
void Game::CreateDeviceAndSwapChain()
{
	// 사용할 SwapChain(구조체)생성 및 초기화 작업
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	{
		// 버퍼 프레임 너비와 높이
		desc.BufferDesc.Width = _width;
		desc.BufferDesc.Height = _height;

		// 화면 재생 빈도(초당 60프레임), 버퍼 갯수 1개로 설정
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;

		// 32비트의 색상 각 8비트씩(RGBA)
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		// 디스플레이 화면을 그릴 때, 스캔라인 순서 여기의 옵션은 화면에 맞춰 자동
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

		// 디스플레이의 비례에 따른 처리로 아래는 아무런 설정을 하지 않음
		// 화면을 늘리거나 줄이거나, 전체화면을 하는 등 후면버퍼와의 비례차가 생길 때 어떻게 하는가에 대한 옵션입니다.
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// 멀티 샘플링 설정 1은 사용하지 않음
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		// 스왑 체인 버퍼의 용도 설정 여기서는 렌더 타겟, 백퍼의 갯수
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;

		// 렌더링할 윈도우 핸들, 창모드 설정
		desc.OutputWindow = _hwnd;
		desc.Windowed = true;

		// 스왑 체인에서 버퍼가 교체되는 방식(아래방식은 마이크로소프에서 권장하는 방식)
		// 여기서의 방식은 DXGI가 호출한 후 백 버퍼의 내용을 삭제하는 방식입니다.
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	// 디바이스와 스왑 체인의 동시 생성
	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,	// 드라이버 타입(디바이스 종류로 여기서는 하드웨어 가속이 목적)
		nullptr,
		0,							// 디바이스 플래그(사용할 DX11의 API레이어)
		nullptr,					// 피처레벨 배열(지원 기능 레벨 배열로 우선순위가 높은 순서대로 배열을 채웁니다)
		0,							// 피처레벨 카운트(개수)
		D3D11_SDK_VERSION,			// SDK 버전
		&desc,						// 스왑체인 생성 구조체 포인터
		_swapChain.GetAddressOf(),	// 생성된 스왑체인 주소
		_device.GetAddressOf(),		// 생성된 디바이스 주소
		nullptr,					// 생성된 피처레벨 정보
		_deviceContext.GetAddressOf()	// 생성된 디바이스 컨텍스트 객체 주소
	);

	// - 포인터 추가 설명
	// Get()은 포인터를 전달, GetAddressOf()는 포인터에서 주소값을 전달하는 방식입니다.
	// GetAddressOf()의 경우에는 포인터 자체를 변경 또는 변수의 값 자체를 변경이 가능합니다.

	CHECK(hr);
}

// 위에서 생성한 후면 버퍼를 복사하는 역할(이름 그대로 렌더 타겟 뷰 생성)
void Game::CreateRenderTargetView()
{
	HRESULT hr;

	// swapChain에서 후면 버퍼를 가져와서 ID3D11Texture2D 리소스를 만들어주고
	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	CHECK(hr);

	// 이후 CreateRenderTargetView를 통해서 GPU에 넘겨줘서 복사하게 합니다.
	_device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
	CHECK(hr);
}

// 화면 크기가 변할 때 한 번만 수행
void Game::SetViewport()
{
	_viewPort.TopLeftX = 0.f;
	_viewPort.TopLeftY = 0.f;
	_viewPort.Width = static_cast<float>(_width);
	_viewPort.Height = static_cast<float>(_height);
	_viewPort.MinDepth = 0.f;
	_viewPort.MaxDepth = 1.f;
}

// 기하 데이터를 생성해 GPU에 전달합니다.(정점 데이터 정의 및 Vertex Buffer 생성)
// 삼각형을 그린다 하면 화면에 그려질 삼각형을 정의하고 GPU에서 사용할 수 있도록 합니다.
void Game::CreateGeometry()
{
	// VertextData 정점 데이터 생성
	{
		_vertices.resize(4);

		_vertices[0].position = Vec3{ -0.5f, -0.5f, 0 };
		_vertices[0].uv = Vec2{ 0.f, 1.f };
		//_vertices[0].color = Color{ 1.f, 0.f, 0.f, 1.f };

		_vertices[1].position = Vec3{ -0.5f, 0.5f, 0 };
		_vertices[1].uv = Vec2{ 0.f, 0.f };
		//_vertices[1].color = Color{ 1.f, 0.f, 0.f, 1.f };

		_vertices[2].position = Vec3{ 0.5f, -0.5f, 0 };
		_vertices[2].uv = Vec2{ 1.f, 1.f };
		//_vertices[2].color = Color{ 1.f, 0.f, 0.f, 1.f };

		_vertices[3].position = Vec3{ 0.5f, 0.5f, 0 };
		_vertices[3].uv = Vec2{ 1.f, 0.f };
		//_vertices[3].color = Color{ 1.f, 0.f, 0.f, 1.f };
	}

	// VertexBuffer (GPU에서 이뤄지는 작업)
	{
		// Buffer 묘사 단계
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;			// 초기화 후 버퍼 데이터를 변경하지 않는 옵션
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 버퍼를 꼭짓점 버퍼로 입력 어셈블러 단계에 바인딩
		desc.ByteWidth = (uint32)sizeof(Vertex) * _vertices.size();

		// Buffer 복사 단계(GPU로 복사할 데이터 설정)
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = _vertices.data();	// 첫 번째 데이터의 시작주소

		// 정점 데이터를 최종적으로 생성합니다.
		HRESULT hr = _device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
		CHECK(hr);
	}

	// index 리스트(버텍스의 위치를 참조해서 구성하며 이를 통해 중복 방지로 메모리를 절약합니다.)
	{
		// 시계 방향
		_indices = { 0, 1, 2, 2, 1, 3 };
	}

	// IndexBuffer
	{
		// Buffer 묘사 단계
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;			// 초기화 후 버퍼 데이터를 변경하지 않는 옵션
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;	// 버퍼를 꼭짓점 버퍼로 입력 어셈블러 단계에 바인딩
		desc.ByteWidth = (uint32)sizeof(uint32) * _indices.size();

		// Buffer 복사 단계(GPU로 복사할 데이터 설정)
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = _indices.data();	// 첫 번째 데이터의 시작주소

		// 정점 데이터를 최종적으로 생성합니다.
		HRESULT hr = _device->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());
		CHECK(hr);
	}
}

// 정점 버퍼에서 데이터를 가져와 CreateInputLayout에서 정의한 시맨틱을 통해 쉐이더로 전달(전달하기 위한 레이아웃 정의)
// 여기서 말하는 레이아웃이란 정점 데이터의 구조를 의미한다고 이해하면 됩니다.
void Game::CreateInputLayout()
{
	// 입력 버퍼 데이터를 설명하는 입력 레이아웃 객체 생성
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		// POSITION은 변수명, COLOR의 12는 바이트 위치(직접 만든 struct 참고)
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 위 구조(배열)를 기준으로 배열의 크기에서 배열 하나의 크기를 나눠서 배열의 개수 구하기
	const int32 count = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	// 이제 최종적으로 레이아웃 객체를 생성하고 이를 통해 정점 데이터를 쉐이더로 전달하게 됩니다.
	_device->CreateInputLayout(layout, count, _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), _inputLayout.GetAddressOf());
}

// 버퍼 데이터 생성
void Game::CreateVS()
{
	LoadShaderFromFile(L"Default.hlsl", "VS", "vs_5_0", _vsBlob);
	HRESULT hr = _device->CreateVertexShader(_vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), nullptr, _vertexShader.GetAddressOf());
	CHECK(hr);
}

// 픽셀 데이터 생성
void Game::CreatePS()
{
	LoadShaderFromFile(L"Default.hlsl", "PS", "ps_5_0", _psBlob);
	HRESULT hr = _device->CreatePixelShader(_psBlob->GetBufferPointer(), _psBlob->GetBufferSize(), nullptr, _pixelShader.GetAddressOf());
	CHECK(hr);
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
	hr = ::CreateShaderResourceView(_device.Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourceView.GetAddressOf());
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

	HRESULT hr = _device->CreateBuffer(&desc, nullptr, _constantBuffer.GetAddressOf());
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
