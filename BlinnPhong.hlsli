float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
    float cosIncidentAngle = saturate(dot(normal, lightVec));

    float f0 = 1.0f - cosIncidentAngle;
    float3 reflectPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);

    return reflectPercent;
}

float3 BF_Schlick(float VdotH, float3 F0, float3 F90)
{
    return F0 + (F90 - F0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

float3 BlinnPhong(float3 lightStrength, float3 lightDir, float3 normal, float3 h, float Roughness, float4 Diffuse, float3 fFactor)
{
    const float ShinyExp = (1 - Roughness) * 256.0f;

    float roughnessFactor = (ShinyExp + 8.0f) * pow(max(dot(h, normal), 0.0f), ShinyExp) / 8.0f;

    float3 specAlbedo = fFactor * roughnessFactor;


    return (Diffuse.rgb + specAlbedo) * lightStrength;
}


float3 ComputeDirectionalLight(float3 lightDir, float3 normal, float lightStrength, float3 view, float Roughness, float4 Diffuse, float3 F0, float3 F90)
{
    float3 n = normalize(normal);
    float3 v = normalize(view);
    float3 l = normalize(lightDir);
    float3 h = normalize(l + v);

    float NdotL = clamp(dot(n, l), 0.0, 1.0);
    float NdotH = clamp(dot(n, h), 0.0, 1.0);


    float3 LightStrength = lightStrength * NdotL;

    float3 FFactor = BF_Schlick(NdotH, F0, F90);

    return BlinnPhong(LightStrength, lightDir, normal, h, Roughness, Diffuse, FFactor);
}