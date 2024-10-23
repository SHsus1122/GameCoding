#pragma once


class Game
{
public:
	Game();
	~Game();

public:
	void Init(HWND hwnd);
	void Update();
	void Render();
	
private:
	void RenderBegin();
	void RenderEnd();

private:
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void SetViewport();

private:
	HWND _hwnd;
	uint32 _width = 0;
	uint32 _height = 0;

private:
	// Device & SwapChain
	// device : 그래픽 리소스를 생성하고 관리하는 객체
	// deviceContext : 그래픽 명령을 설정하고 실행하는 인터페이스
	// swapChain : 화면에 렌더링된 이미지를 표시하기 위한 매커니즘
	ComPtr<ID3D11Device> _device = nullptr;;
	ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;;
	ComPtr<IDXGISwapChain> _swapChain = nullptr;

	// RTV(Render Target View)
	ComPtr<ID3D11RenderTargetView> _rednerTargetView;

	// Misc
	D3D11_VIEWPORT _viewPort = { 0 };	// 렌더타겟으로 만든 녀석의 화면 크기 복사용
	float _clearColor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
};

