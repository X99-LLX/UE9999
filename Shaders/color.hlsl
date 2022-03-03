//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
	float4x4 gScale3D;
	float4x4 gRotate;
};

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
	float4 Normal : NORMAL;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
	float4 Normal : NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	//vout.PosH = mul(float4(vin.PosL, 1.0f), gRotate);
	float4 ScalePos =mul(mul(float4(vin.PosL, 1.0f), gScale3D), gRotate);
	//float4 RotatePos = mul(ScalePos, gScale3D);
	vout.PosH = mul(ScalePos, gWorldViewProj);
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;

	vout.Normal = vin.Normal;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return pin.Normal;
}


