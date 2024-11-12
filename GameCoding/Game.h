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
	void CreateGeometry();
	void CreateInputLayout();
	void CreateVS();
	void CreatePS();

	void CreateResterizerState();
	void CreateSamplerState();
	void CreateBlendState();
	void CreateSRV();

	void CreateConstantBuffer();

	void LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob);

private:
	HWND _hwnd;
	//uint32 _width = 0;
	//uint32 _height = 0;

	Graphics* _graphics;

private:
	// [ Geometry ]
	// VertexBuffer
	vector<Vertex> _vertices;
	VertexBuffer* _vertexBuffer;
	//ComPtr<ID3D11Buffer> _vertexBuffer = nullptr;

	// IndexBuffer
	vector<uint32> _indices;
	IndexBuffer* _indexBuffer;

	// InputLayout
	InputLayout* _inputLayout;
	//ComPtr<ID3D11InputLayout> _inputLayout = nullptr;

	// ID3DBlob인터페이스는 버전 중립적(모든 D3D버전에서 사용 가능)이며,
	// 메시 최적화 및 로드 작업 중에 꼭짓점, 인접성 및 재료 정보를 저장하는 데이터 버퍼로 사용 가능합니다.

	// VS
	ComPtr<ID3D11VertexShader> _vertexShader = nullptr;
	ComPtr<ID3DBlob> _vsBlob = nullptr;

	// RS
	ComPtr<ID3D11RasterizerState> _rasterizerState = nullptr;

	// PS
	ComPtr<ID3D11PixelShader> _pixelShader = nullptr;
	ComPtr<ID3DBlob> _psBlob = nullptr;

	// SRV
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView = nullptr;

	ComPtr<ID3D11SamplerState> _samplerState = nullptr;
	ComPtr<ID3D11BlendState> _blendState = nullptr;
	// [ CPU<->RAM ] [ GPU<->VRAM ]
private:
	// SRT
	TransformData _transformData;
	ComPtr<ID3D11Buffer> _constantBuffer;

	Vec3 _localPosition = { 0.f, 0.f, 0.f };
	Vec3 _localRotation = { 0.f, 0.f, 0.f };
	Vec3 _localScale = { 1.f, 1.f, 1.f };
};

