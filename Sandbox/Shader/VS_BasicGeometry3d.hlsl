cbuffer CBuffer
{
	matrix WorldViewProjMatrix;
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.pos = mul(float4(input.pos, 1.0f), WorldViewProjMatrix);
	output.tex = input.tex;

	//output.normal = (float3)mul(float4(input.normal, 0.0f), World);
	return output;
}