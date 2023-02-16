static const int MAX_INSTSANCES = 1000;
static const int MAX_BONES = 1000;

cbuffer ConstantsForInstancing : register(b0)
{
	matrix Worlds[MAX_INSTSANCES];
};

cbuffer ConstantsOther : register(b1)
{
	matrix ViewProjection;
	matrix DEBUGSCALAR;
}

cbuffer ConstantsForBlending : register(b2)
{
	matrix Bones[MAX_BONES];
}

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 tex : TEXCOORD;
	float3 normal : NORMAL;
	uint4  blendIndices : BLENDINDICES;
	float4 blendWeights : BLENDWEIGHT;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	uint InstanceId : SV_InstanceID;
};

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID, uint InstanceId : SV_InstanceID)
{
	VS_OUTPUT output;
	float4 accPosition = float4(0.0F, 0.0F, 0.0F, 0.0F);
	float3 accNormal = float3(0.0F, 0.0F, 0.0F);

	for (int i = 0; i < 4; i++)
	{
		const float  blendWeight  = input.blendWeights[i];
		const int affectingBoneId = input.blendIndices[i];
		if (blendWeight < 0.00001F) { break; }
		
		const matrix boneTransform = Bones[affectingBoneId];
		

		const float4 localPosition = mul(float4(input.pos, 1.0F), boneTransform) * blendWeight/**/;

		accPosition += localPosition;


		const float3 localNormal = mul(float4(input.normal, 0.0F), boneTransform).xyz;

		accNormal += localNormal;
	}

//	accPosition.w = 1.0F;
	// if w sum is not 1.0F, normalize

	if (accPosition.w != 0.0F)
	{
		accPosition /= accPosition.w;
	}
	else
	{
		accPosition = float4(input.pos, 1.0F);
		accNormal = input.normal;
	}
	
	

	const matrix World = Worlds[InstanceId];

	output.pos		  = mul(accPosition, mul(World, ViewProjection)); // mul pos * W * V * P
	output.tex		  = input.tex.xy;
	output.normal = mul(float4(normalize(accNormal), 0.0F), World).xyz;
	output.InstanceId = InstanceId;
	return output;
}