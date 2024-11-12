#pragma once

class VertexBuffer
{
public:
	VertexBuffer(ComPtr<ID3D11Device> device);
	~VertexBuffer();

	ComPtr<ID3D11Buffer> GetComPtr() { return _vertexBuffer; }
	uint32 GetStride() { return _stride; }
	uint32 GetOffset() { return _offset; }
	uint32 GetCount() { return _count; }

	// VertexBuffer (GPU에서 이뤄지는 작업)
	template<typename T>
	void Create(const vector<T>& vertices)
	{
		_stride = sizeof(T);	// 하나의 정점 데이터가 차지하는 바이트의 크기
		_count = static_cast<uint32>(vertices.size());	// 정점의 개수

		// Buffer 묘사 단계
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;			// 초기화 후 버퍼 데이터를 변경하지 않는 옵션 GPU 에서 리드만 가능한 상태
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 버퍼를 꼭짓점 버퍼로 입력 어셈블러 단계에 바인딩
		desc.ByteWidth = (uint32)(_stride * _count);

		// Buffer 복사 단계(GPU로 복사할 데이터 설정)
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = vertices.data();	// 첫 번째 데이터의 시작주소

		// 정점 데이터를 최종적으로 생성합니다.
		HRESULT hr = _device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
		CHECK(hr);
	}

private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11Buffer> _vertexBuffer;

	uint32 _stride = 0;
	uint32 _offset = 0;
	uint32 _count = 0;
};

