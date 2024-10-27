// HLSL(High Level Shader Language)
// 고급 쉐이더 언어라는 의미로 다이렉트3D API 에서 사용되는 쉐이딩 언어입니다.
// Vertex Shader(정점 쉐이더)
// 물체를 이루는 각 정점들의 위치를 화면좌표로 변환합니다.


// 정점 쉐이더에 입력되는 데이터 구조체
struct VS_INPUT
{
    // CreateInputLayout에서 만든 layout구조체에서 지정한 변수명 POSITION 으로 
    float4 position : POSITION;
    //float4 color : COLOR;
    float2 uv : TEXCOORD;
};

// 정점 쉐이더가 출력하는 데이터 구조체
struct VS_OUTPUT
{
    // SV_POSITION 이는 최종 변환 위치로 D3D는 이 데이터를 기반으로 좌표 변환을 처리합니다.
    float4 position : SV_POSITION;
    //float4 color : COLOR;
    float2 uv : TEXCOORD;
};

// IA - VS - RS - PS - OM
// VertexShader 메인 함수
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = input.position;
    output.uv = input.uv;

    return output;
}

// t0 레지스터에 texture0 을 등록합니다.
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target
{
    // texture0 에 uv 좌표를 이용해서 그에 해당하는 색상을 빼온다는 의미입니다.
    float4 color = texture0.Sample(sampler0, input.uv);

    return color;
}