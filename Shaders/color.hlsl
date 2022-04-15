#include "CommonH.hlsli"
#include "BRDF.hlsli"
#include "BlinnPhong.hlsli"

Texture2D	gDiffuseMap	:	register(t0);

Texture2D	gShadowMap	:	register(t1);

Texture2D	gNormalMap	:	register(t2);

SamplerState	gSamLinear	:	register(s0);

SamplerComparisonState gsamShadow : register(s1);

float4	CameraLoc	:	register(b0);

cbuffer cbPerObject : register(b1)
{
	float4x4	LightTVP;
	float4x4	LightMVP;
	float4x4	gWorld;
	float4x4	CameraMVP;
	float4x4	gScale3D;
	float4x4	gRotate;
	float		gOffset;
};

cbuffer MatData : register(b2)
{
	float4		gBaseColor;
	float3		gFresnelRO;
	float		gMetallic;
	float		gSpecular;
	float		gRoughness;
};

cbuffer LightData : register(b3)
{
	float3		gLightPosition;
	float		gLightIntensity;
	float3		gLightColor;
	float		gLightRadius;
};

struct VertexIn
{
	float3 PosL			: POSITION;
    float4 Tangent		: COLOR;
	float4 Normal		: NORMAL;
	float2 TexCoord		: TEXCOORD;
};

struct VertexOut
{
	float4 PosH			: SV_POSITION;
    float3 Tangent		: COLOR;
	float3 Normal		: NORMAL;
	float2 TexCoord		: TEXCOORD;
	float4 ShadowPos	: SHADOWPOS;
	float3 WorldPos		: POSITION1;
};

float CalcShadowFactor(float4 shadowPosH)
{
	shadowPosH.xyz /= shadowPosH.w;

	float depth = shadowPosH.z;

	uint width, height, numMips;
	gShadowMap.GetDimensions(0, width, height, numMips);

	float dx = 1.0f / (float)width;

	float percentLit = 0.0f;
	const float2 offsets[9] =
	{
		float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
	};

	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		percentLit += gShadowMap.SampleCmpLevelZero(gsamShadow, shadowPosH.xy + offsets[i], depth).r;
	}

	return percentLit / 9.0f;
}

[RootSignature(Color_RootSig)]

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = mul(float4(vin.PosL, 1.0f), CameraMVP);

	vout.Tangent = mul(normalize(vin.Tangent.xyz), (float3x3)gWorld);
	vout.Normal = mul(float4(vin.Normal.xyz, 0), gWorld).xyz;
	vout.TexCoord = vin.TexCoord;

	vout.ShadowPos = mul(mul(float4(vin.PosL, 1.0f), gWorld), LightTVP);

	vout.WorldPos = mul(float4(vin.PosL, 1.0f), gWorld);

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 Output = 0.0f;

	float4 BaseColor = gDiffuseMap.Sample(gSamLinear  , pin.TexCoord);
	/*float4 BaseColor = float4(pin.Normal, 1.0f);*/
	BaseColor.rgb *= gBaseColor.rgb;

	float Shadow = CalcShadowFactor(pin.ShadowPos);

	float4 NormalMapSample = gNormalMap.Sample(gSamLinear, pin.TexCoord);

	float3 bumpedNormalW = NormalSampleToWorldSpace(NormalMapSample.rgb, pin.Normal, pin.Tangent);

	BaseColor.rgb *= gBaseColor.rgb;

	float Roughness = gRoughness;
	float Metallic = gMetallic;
	float F0 = 0.74f;
	float AO = 20.0f;
	F0 = lerp(F0.rrr, BaseColor.rgb, Metallic);

	float3 Ambient = 0.5f * gLightColor;

	SurfaceInfo surfaceInfo = GetSurfaceInfo(BaseColor, Metallic, Roughness);

	Output.rgb += ComputeDirectionalLight(gLightPosition - pin.WorldPos, bumpedNormalW, 1.f, CameraLoc.xyz - pin.WorldPos, Roughness, BaseColor, surfaceInfo.F0, surfaceInfo.F90) * Shadow;
	Output.rgb += Ambient * BaseColor.rgb;
	Output.rgb = pow(Output.rgb, 3);
	return Output;
	/*{
		float3 PointLightPos = gLightPosition;
		float LightRadius = gLightRadius;
		float LightStrenth = gLightIntensity;

		float3 WPos = pin.WorldPos;
		float FallOff = distance(PointLightPos, WPos);
		FallOff = LightRadius / (FallOff * FallOff);
		float3 V = normalize(CameraLoc.xyz - pin.WorldPos);
		float3 N = bumpedNormalW;
		float3 L = normalize(PointLightPos - WPos);
		float3 H = normalize(V + L);
		float3 R = -reflect(V, N);

		float NoL = saturate(dot(L, N));
		float NoH = saturate(dot(N, H));
		float NoV = saturate(dot(N, V));
		float VoH = saturate(dot(V, H));
		float NoR = saturate(dot(N, R));

		float3 Diffuse = Diffuse_Burley(BaseColor.rgb, Roughness, NoV, NoL, NoH);

		float a2 = Roughness * Roughness * Roughness * Roughness;
		float D = D_GGX(a2, NoH);
		float G = Vis_SmithJointApprox(a2, NoV, NoL);
		float F = FSchlick(VoH, F0);
		float3 Specular = D * G * F;
		Output.rgb += (Diffuse + Specular) * NoL * Shadow * (FallOff * LightStrenth) * 300;
	}

	Output.rgb += 0.03f * BaseColor.rgb * AO;

    return pow(Output, 1 / 1.2f);*/
}


