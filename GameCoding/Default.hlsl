// HLSL(High Level Shader Language)
// ��� ���̴� ����� �ǹ̷� ���̷�Ʈ3D API ���� ���Ǵ� ���̵� ����Դϴ�.
// Vertex Shader(���� ���̴�)
// ��ü�� �̷�� �� �������� ��ġ�� ȭ����ǥ�� ��ȯ�մϴ�.


// ���� ���̴��� �ԷµǴ� ������ ����ü
struct VS_INPUT
{
    // CreateInputLayout���� ���� layout����ü���� ������ ������ POSITION ���� 
    float4 position : POSITION;
    //float4 color : COLOR;
    float2 uv : TEXCOORD;
};

// ���� ���̴��� ����ϴ� ������ ����ü
struct VS_OUTPUT
{
    // SV_POSITION �̴� ���� ��ȯ ��ġ�� D3D�� �� �����͸� ������� ��ǥ ��ȯ�� ó���մϴ�.
    float4 position : SV_POSITION;
    //float4 color : COLOR;
    float2 uv : TEXCOORD;
};

// IA - VS - RS - PS - OM
// VertexShader ���� �Լ�
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = input.position;
    output.uv = input.uv;

    return output;
}

// t0 �������Ϳ� texture0 �� ����մϴ�.
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target
{
    // texture0 �� uv ��ǥ�� �̿��ؼ� �׿� �ش��ϴ� ������ ���´ٴ� �ǹ��Դϴ�.
    float4 color = texture0.Sample(sampler0, input.uv);

    return color;
}