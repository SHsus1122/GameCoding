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

	// TODO
	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
	SetViewport();
}

void Game::Update()
{

}

void Game::Render()
{
	RenderBegin();



	RenderEnd();
}

void Game::RenderBegin()
{
	// 화면을 그려주고, 비워주고, 뷰포트에 설정
	_deviceContext->OMSetRenderTargets(1, _rednerTargetView.GetAddressOf(), nullptr);
	_deviceContext->ClearRenderTargetView(_rednerTargetView.Get(), _clearColor);
	_deviceContext->RSSetViewports(1, &_viewPort);
}

void Game::RenderEnd()
{
	// Present : 다 그린 전면 버퍼를 복사 즉, 출력하는 명령어
	HRESULT hr = _swapChain->Present(1, 0);
	CHECK(hr);
}

// 화면 버퍼 만들기 작업
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

// 위에서 생성한 후면 버퍼를 복사하는 역할
void Game::CreateRenderTargetView()
{
	HRESULT hr;

	// swapChain에서 후면 버퍼에 해당하는 리소스를 ID3D11Texture2D로 만들어주고
	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	CHECK(hr);

	// 이후 CreateRenderTargetView를 통해서 GPU에 넘겨줘서 복사하게 합니다.
	_device->CreateRenderTargetView(backBuffer.Get(), nullptr, _rednerTargetView.GetAddressOf());
	CHECK(hr);
}

// 화면 크기에 변할 때 한 번만 수행
void Game::SetViewport()
{
	_viewPort.TopLeftX = 0.f;
	_viewPort.TopLeftY = 0.f;
	_viewPort.Width = static_cast<float>(_width);
	_viewPort.Height = static_cast<float>(_height);
	_viewPort.MinDepth = 0.f;
	_viewPort.MaxDepth = 1.f;
}
