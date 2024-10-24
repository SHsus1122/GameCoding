// HLSL(High Level Shader Language)
// ��� ���̴� ����� �ǹ̷� ���̷�Ʈ3D API ���� ���Ǵ� ���̵� ����Դϴ�.
// Vertex Shader(���� ���̴�)
// ��ü�� �̷�� �� �������� ��ġ�� ȭ����ǥ�� ��ȯ�մϴ�.


// ���� ���̴��� �ԷµǴ� ������ ����ü
struct VS_INPUT
{
    // CreateInputLayout���� ���� layout����ü���� ������ ������ POSITION ���� 
    float4 position : POSITION;
    float4 color : COLOR;
};

// ���� ���̴��� ����ϴ� ������ ����ü
struct VS_OUTPUT
{
    // SV_POSITION �̴� ���� ��ȯ ��ġ�� D3D�� �� �����͸� ������� ��ǥ ��ȯ�� ó���մϴ�.
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// IA - VS - RS - PS - OM
// VertexShader ���� �Լ�
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = input.position;
    output.color = input.color;
    
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    
    
    return input.color;
}