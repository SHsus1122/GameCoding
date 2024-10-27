#pragma once
#include "Types.h"


struct Vertex
{
	Vec3 position;	// 0+,  12바이트 사이즈

	//Color color;	// 12+, 12바이트 사이즈


	// 아래의 UV와 UV매핑은 컴퓨터 그래픽스에서 3D모델에 텍스처를 입히기 위한 중요한 개념입니다.
	// UV : 3D모델에서 텍스처 좌표계를 나타내는 용어입니다.
	//		3D모델의 표면을 2D평면으로 표현하기 위해 사용되는 좌표 시스템입니다.
	// UV Mapping : 3D모델의 표면에 2D이미지를 입히는(매핑하는) 과정을 말합니다.
	// - U : 가로, V : 세로
	Vec2 uv;
};

struct TransformData
{
	Vec3 offset;
	float dummy;	// ConstantBuffer를 생성하기 위해 16바이트 정렬을 위한 더미 데이터
};