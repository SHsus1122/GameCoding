#include "pch.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(ComPtr<ID3D11Device> device)
	: _device(device)
{

}

IndexBuffer::~IndexBuffer()
{

}

void IndexBuffer::Create(const vector<uint32>& indices)
{
	_stride = sizeof(uint32);
	_count = static_cast<uint32>(indices.size());

	// Buffer 묘사 단계
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_IMMUTABLE;			// 초기화 후 버퍼 데이터를 변경하지 않는 옵션
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;	// 버퍼를 꼭짓점 버퍼로 입력 어셈블러 단계에 바인딩
	desc.ByteWidth = (uint32)(_stride * _count);

	// Buffer 복사 단계(GPU로 복사할 데이터 설정)
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = indices.data();	// 첫 번째 데이터의 시작주소

	// 정점 데이터를 최종적으로 생성합니다.
	HRESULT hr = _device->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());
	CHECK(hr);
}
