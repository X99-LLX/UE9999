#ifndef __BRDF_HLSLI__
#define __BRDF_HLSLI__

#ifndef PI
#   define  PI 3.141592653589793
#endif

struct SurfaceInfo
{
    float  PerceptualRoughness;
    float3 F0;
    float3 F90;
    float3 DiffuseColor;
};

struct AngularInfo
{
    float NdotL;
    float NdotV;
    float NdotH;
    float LdotH;
    float VdotH;
};

SurfaceInfo GetSurfaceInfo(float4 BaseColor, float4 PhysicalDesc)
{
    SurfaceInfo surfaceInfo;

    float metallic = PhysicalDesc.b;

    float3 noMetalF0 = float3(0.04, 0.04, 0.04);

    surfaceInfo.PerceptualRoughness = clamp(PhysicalDesc.g, 0.0, 1.0);

    surfaceInfo.DiffuseColor = BaseColor.rgb * (float3(1.0, 1.0, 1.0) - noMetalF0) * (1.0 - metallic);

    surfaceInfo.F0 = lerp(noMetalF0, BaseColor.rgb, metallic);
    float reflectance = max(max(surfaceInfo.F0.r, surfaceInfo.F0.g), surfaceInfo.F0.b);

    surfaceInfo.F90 = clamp(reflectance * 50.0, 0.0, 1.0) * float3(1.0, 1.0, 1.0);

    return surfaceInfo;
}

SurfaceInfo GetSurfaceInfo(float4 BaseColor, float Metallic, float Roughness)
{
    SurfaceInfo surfaceInfo;

    float metallic = Metallic;

    float3 noMetalF0 = float3(0.04, 0.04, 0.04);

    surfaceInfo.PerceptualRoughness = Roughness;

    surfaceInfo.DiffuseColor = BaseColor.rgb * (float3(1.0, 1.0, 1.0) - noMetalF0) * (1.0 - metallic);

    surfaceInfo.F0 = lerp(noMetalF0, BaseColor.rgb, metallic);
    float reflectance = max(max(surfaceInfo.F0.r, surfaceInfo.F0.g), surfaceInfo.F0.b);

    surfaceInfo.F90 = clamp(reflectance * 50.0, 0.0, 1.0) * float3(1.0, 1.0, 1.0);

    return surfaceInfo;
}

// Lambertian Diffuse
float3 LambertianDiffuse(float3 DiffuseColor)
{
    return DiffuseColor / PI;
}

// Schlick Fresnel Reflection
float3 F_Schlick(float VdotH, float3 F0, float3 F90)
{
    return F0 + (F90 - F0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

float3 FSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// [Beckmann 1963, "The scattering of electromagnetic waves from rough surfaces"]
float D_Beckmann(float a2, float NoH)
{
    float NoH2 = NoH * NoH;
    return exp((NoH2 - 1) / (a2 * NoH2)) / (PI * a2 * NoH2 * NoH2);
}

// GGX / Trowbridge-Reitz
// [Walter et al. 2007, "Microfacet models for refraction through rough surfaces"]
float D_GGX(float a2, float NoH)
{
    float d = (NoH * a2 - NoH) * NoH + 1;	// 2 mad
    return a2 / (PI * d * d);					// 4 mul, 1 rcp
}

// Visibility = G(v,l,a) / (4 * (n,v) * (n,l))
// see https://google.github.io/filament/Filament.md.html#materialsystem/specularbrdf/geometricshadowing(specularg)
float SmithGGXVisibilityCorrelated(float NdotL, float NdotV, float AlphaRoughness)
{
    float a2 = AlphaRoughness * AlphaRoughness;

    float GGXV = NdotL * sqrt(max(NdotV * NdotV * (1.0 - a2) + a2, 1e-7));
    float GGXL = NdotV * sqrt(max(NdotL * NdotL * (1.0 - a2) + a2, 1e-7));

    return 0.5 / (GGXV + GGXL);
}

float Vis_SmithJointApprox(float a2, float NoV, float NoL)
{
    float a = sqrt(a2);
    float Vis_SmithV = NoL * (NoV * (1 - a) + a);
    float Vis_SmithL = NoV * (NoL * (1 - a) + a);
    return 0.5 * rcp(Vis_SmithV + Vis_SmithL);
}

float Pow5(float val)
{
    return val * val * val * val * val;
}

float3 Diffuse_Burley(float3 DiffuseColor, float Roughness, float NoV, float NoL, float VoH)
{
    float FD90 = 0.5 + 2 * VoH * VoH * Roughness;
    float FdV = 1 + (FD90 - 1) * Pow5(1 - NoV);
    float FdL = 1 + (FD90 - 1) * Pow5(1 - NoL);
    return DiffuseColor * ((1 / PI) * FdV * FdL);
}

float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
    float3 normalT = 2.0f * normalMapSample - 1.0f;

    //    float3 N = normalize(unitNormalW);
    //    float3 T = normalize(tangentW - dot(tangentW, N) * N);
    //    float3 B = normalize(cross(N, T));

    float3 N = unitNormalW;
    float3 T = normalize(tangentW - N * tangentW * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

    float3 NormalW = normalize(mul(normalT, TBN));

    return NormalW;
}

float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW, float3 tangentY)
{
    float3 normalT = 2.0f * normalMapSample - 1.0f;

    float3 N = unitNormalW;
    float3 T = tangentW;
    float3 B = tangentY;

    float3x3 TBN = float3x3(T, B, N);

    float3 NormalW = normalize(mul(normalT, TBN));

    return NormalW;
}

void BRDF(AngularInfo angularInfo, SurfaceInfo surfaceInfo, out float3 diffuseContrib, out float3 specContrib)
{
    diffuseContrib = float3(0.0, 0.0, 0.0);
    specContrib = float3(0.0, 0.0, 0.0);

    float AlphaRoughness = surfaceInfo.PerceptualRoughness * surfaceInfo.PerceptualRoughness;

    float3 F = F_Schlick(angularInfo.VdotH, surfaceInfo.F0, surfaceInfo.F90);
    float D = D_GGX(AlphaRoughness * AlphaRoughness, angularInfo.NdotH);
    //    float G = SmithGGXVisibilityCorrelated(angularInfo.NdotL, angularInfo.NdotV, AlphaRoughness);
    float G = Vis_SmithJointApprox(AlphaRoughness * AlphaRoughness, angularInfo.NdotV, angularInfo.NdotL);

    diffuseContrib = (1.0 - F) * LambertianDiffuse(surfaceInfo.DiffuseColor);
    specContrib = D * G * F;
}

float3 ApplyDirectionalLight(float3 lightDir, float3 lightColor, SurfaceInfo surfaceInfo, float3 normal, float3 view)
{
    float3 n = normalize(normal);
    float3 v = normalize(view);
    float3 l = normalize(lightDir);
    float3 h = normalize(l + v);

    AngularInfo angularInfo;
    angularInfo.NdotL = clamp(dot(n, l), 0.0, 1.0);
    angularInfo.NdotV = clamp(dot(n, v), 0.0, 1.0);
    angularInfo.NdotH = clamp(dot(n, h), 0.0, 1.0);
    angularInfo.LdotH = clamp(dot(l, h), 0.0, 1.0);
    angularInfo.VdotH = clamp(dot(v, h), 0.0, 1.0);

    float3 diffuseContrib, specContrib;

    BRDF(angularInfo, surfaceInfo, diffuseContrib, specContrib);

    float3 shade = (diffuseContrib + specContrib) * angularInfo.NdotL;

    return shade * lightColor;
}

#endif