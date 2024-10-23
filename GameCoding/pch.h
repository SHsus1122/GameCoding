#pragma once
// Precomplied Header
// 미리 컴파일한 헤더라는 의미로 컴파일 시간을 효과적으로 줄이기 위해 사용합니다.
// 이는 자주 변경되지 않는 긴소스를 미리 컴파일해 컴파일결과를 별도의 파일에 저장하여 똑같은 헤더를 컴파일시 해당파일을 컴파일 하지 않고 미리 컴파일된 헤더 파일을 사용해 컴파일 속도를 향상시켜줍니다.


#include "Types.h"
#include "Values.h"
#include "Struct.h"

// STL
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
using namespace std;

// WIN
#include <Windows.h>
#include <assert.h>

// DX
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>
using namespace DirectX;
using namespace Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

// DirectXTex 라이브러리
// 이는 텍스처 리소스를 보다 편하게 관리하고 사용할 수 있는 기능을 제공합니다.
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif

#define CHECK(p)	assert(SUCCEEDED(p))