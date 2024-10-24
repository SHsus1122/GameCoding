#pragma once
#include <string>


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
	void CreateGeometry();
	void CreateInputLayout();

	void CreateVS();
	void CreatePS();

	void LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob);

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
	ComPtr<ID3D11RenderTargetView> _renderTargetView;

	// Misc
	D3D11_VIEWPORT _viewPort = { 0 };	// 렌더타겟으로 만든 녀석의 화면 크기 복사용
	float _clearColor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };

private:
	// Geometry
	vector<Vertex> _vertices;
	ComPtr<ID3D11Buffer> _vertexBuffer = nullptr;
	ComPtr<ID3D11InputLayout> _inputLayout = nullptr;

	// ID3DBlob인터페이스는 버전 중립적(모든 D3D버전에서 사용 가능)이며,
	// 메시 최적화 및 로드 작업 중에 꼭짓점, 인접성 및 재료 정보를 저장하는 데이터 버퍼로 사용 가능합니다.
	
	// VS
	ComPtr<ID3D11VertexShader> _vertexShader = nullptr;
	ComPtr<ID3DBlob> _vsBlob = nullptr;

	// PS
	ComPtr<ID3D11PixelShader> _pixelShader = nullptr;
	ComPtr<ID3DBlob> _psBlob = nullptr;

	// [ CPU<->RAM ] [ GPU<->VRAM ]

};

