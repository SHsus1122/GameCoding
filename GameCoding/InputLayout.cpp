#include "pch.h"
#include "InputLayout.h"

InputLayout::InputLayout(ComPtr<ID3D11Device> device)
	:_device(device)
{
}

InputLayout::~InputLayout()
{
}

void InputLayout::Create(const vector<D3D11_INPUT_ELEMENT_DESC>& descs, ComPtr<ID3DBlob> blob)
{
	// 위 구조(배열)를 기준으로 배열의 크기에서 배열 하나의 크기를 나눠서 배열의 개수 구하기
	const int32 count = static_cast<int32>(descs.size());

	// 이제 최종적으로 레이아웃 객체를 생성하고 이를 통해 정점 데이터를 쉐이더로 전달하게 됩니다.
	_device->CreateInputLayout(descs.data(), count, blob->GetBufferPointer(), blob->GetBufferSize(), _inputLayout.GetAddressOf());
}
