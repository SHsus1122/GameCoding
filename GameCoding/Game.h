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

	void CreateSRV();

	void CreateConstantBuffer();

	void LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob);

private:
	HWND _hwnd;
	uint32 _width = 0;
	uint32 _height = 0;


	// COM(Component Obejct Model)
	// 이는 어떤 프로그램이나 시스템을 이루는 컴포넌트들이 상호 통신할 수 있도록 하는 메커니즘을 말합니다.
	// 아래에서 사용한 ID3D11.. 에 해당하며 이들은 COM객체 또는 COM인터페이스 라고도 합니다.
	// 이들 덕분에 컴퓨터마다 사양이 다 다른데 일일이 대응하는 것이 아닌 각자 다른 환경에서 문제없이 잘 돌아가게끔 편리하게 해줍니다. 2219

private:
	// [ Device & SwapChain ]
	// device : 그래픽 리소스를 생성하고 관리하는 객체
	// deviceContext : 그래픽 명령을 설정하고 실행하는 인터페이스
	// swapChain : 화면에 렌더링된 이미지를 표시하기 위한 매커니즘
	ComPtr<ID3D11Device> _device = nullptr;;
	ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;;
	ComPtr<IDXGISwapChain> _swapChain = nullptr;

	// RTV(Render Target View)
	// 렌더링 파이프라인의 출력을 특정한 리소스와 바인딩시 사용합니다.
	ComPtr<ID3D11RenderTargetView> _renderTargetView;

	// Misc
	D3D11_VIEWPORT _viewPort = { 0 };	// 렌더타겟으로 만든 녀석의 화면 크기 복사용
	float _clearColor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };

private:
	// [ Geometry ]
	// VertexBuffer
	vector<Vertex> _vertices;
	ComPtr<ID3D11Buffer> _vertexBuffer = nullptr;

	// IndexBuffer
	vector<uint32> _indices;
	ComPtr<ID3D11Buffer> _indexBuffer = nullptr;

	// InputLayout
	ComPtr<ID3D11InputLayout> _inputLayout = nullptr;

	// ID3DBlob인터페이스는 버전 중립적(모든 D3D버전에서 사용 가능)이며,
	// 메시 최적화 및 로드 작업 중에 꼭짓점, 인접성 및 재료 정보를 저장하는 데이터 버퍼로 사용 가능합니다.
	
	// VS
	ComPtr<ID3D11VertexShader> _vertexShader = nullptr;
	ComPtr<ID3DBlob> _vsBlob = nullptr;

	// PS
	ComPtr<ID3D11PixelShader> _pixelShader = nullptr;
	ComPtr<ID3DBlob> _psBlob = nullptr;

	// SRV
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView = nullptr;

	// [ CPU<->RAM ] [ GPU<->VRAM ]
private:
	// SRT
	TransformData _transformData;
	ComPtr<ID3D11Buffer> _constantBuffer;
};

