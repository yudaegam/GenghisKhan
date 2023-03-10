static const int MAX_INSTSANCES = 1000;
static const int MAX_BONES = 100;

cbuffer ConstantsForInstancing : register(b0)
{
	matrix Worlds[MAX_INSTSANCES];
};

cbuffer ConstantsOthers : register(b1)
{
	matrix ViewProjection;
}

cbuffer ConstantsForBlending : register(b2)
{
	uint4 StartBoneTransformLocations[MAX_INSTSANCES / 4];

}

StructuredBuffer<matrix> FinalBoneTransforms : register(t0);

struct VS_IN
{
	float3 pos : POSITION;
	float3 tex : TEXCOORD;
	float3 normal : NORMAL;
	uint4  blendIndices : BLENDINDICES;
	float4 blendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	uint InstanceId : SV_InstanceID;
};

VS_OUT main(VS_IN input, uint vertexID : SV_VertexID, uint InstanceId : SV_InstanceID)
{
	VS_OUT output;
	float4 accPosition = float4(0.0F, 0.0F, 0.0F, 0.0F);
	float3 accNormal = float3(0.0F, 0.0F, 0.0F);

	for (int i = 0; i < 4; i++)
	{
		const float  blendWeight  = input.blendWeights[i];
		const int affectingBoneId = input.blendIndices[i];

		if (blendWeight == 0.0F) { break; } // early break, but this is not neccessary, without this, shader work correctly
		
		const matrix boneTransform = FinalBoneTransforms[StartBoneTransformLocations[InstanceId/4][InstanceId%4] + affectingBoneId] * blendWeight;
		

		const float4 localPosition = mul(float4(input.pos, 1.0F), boneTransform);

		accPosition += localPosition;


		const float3 localNormal = mul(float4(input.normal, 0.0F), boneTransform).xyz;

		accNormal += localNormal;
	}

	// w sum always 1.0F, if not, something was wrong, this fact is absolutly true, never suspect that, you are wrong, assimp is no guilty
	if (accPosition.w == 0.0F)
	{
		accPosition = float4(input.pos, 1.0F);
		accNormal = input.normal;
	}



	const matrix World = Worlds[InstanceId];

	output.pos		  = mul(accPosition, mul(World, ViewProjection)); // mul pos * W * V * P
	output.tex		  = input.tex.xy;
	output.normal = normalize(mul(float4(accNormal, 0.0F), World).xyz);
	output.InstanceId = InstanceId;
	return output;
}