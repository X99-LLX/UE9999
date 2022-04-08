//***************************************************************************************
// d3dUtil.h by Frank Luna (C) 2015 All Rights Reserved.
//
// General helper code.
//***************************************************************************************

#pragma once

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include "d3dx12.h"


extern const int gNumFrameResources;

inline void d3dSetDebugName(IDXGIObject* obj, const char* name)
{
    if (obj)
    {
        obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
    }
}
inline void d3dSetDebugName(ID3D12Device* obj, const char* name)
{
    if (obj)
    {
        obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
    }
}
inline void d3dSetDebugName(ID3D12DeviceChild* obj, const char* name)
{
    if (obj)
    {
        obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
    }
}

inline std::wstring AnsiToWString(const std::string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}

/*
#if defined(_DEBUG)
    #ifndef Assert
    #define Assert(x, description)                                  \
    {                                                               \
        static bool ignoreAssert = false;                           \
        if(!ignoreAssert && !(x))                                   \
        {                                                           \
            Debug::AssertResult result = Debug::ShowAssertDialog(   \
            (L#x), description, AnsiToWString(__FILE__), __LINE__); \
        if(result == Debug::AssertIgnore)                           \
        {                                                           \
            ignoreAssert = true;                                    \
        }                                                           \
                    else if(result == Debug::AssertBreak)           \
        {                                                           \
            __debugbreak();                                         \
        }                                                           \
        }                                                           \
    }
    #endif
#else
    #ifndef Assert
    #define Assert(x, description)
    #endif
#endif
    */

class d3dUtil
{
public:

    static bool IsKeyDown(int vkeyCode);

	static glm::vec4 Vector3TransformCoord(glm::vec3 V, glm::mat4 M) {
		glm::vec4 Z(V.z, V.z, V.z, V.z);
		glm::vec4 Y(V.y, V.y, V.y, V.y);
		glm::vec4 X(V.x, V.x, V.x, V.x);

		glm::vec4 Result = Z * M[2] + M[3];
		Result = Y * M[1] + Result;
		Result = X * M[0] + Result;
		glm::vec4 W(Result.w, Result.w, Result.w, Result.w);

		return Result / W;
	}


    static std::string ToString(HRESULT hr);

    static UINT CalcConstantBufferByteSize(UINT byteSize)
    {
        // Constant buffers must be a multiple of the minimum hardware
        // allocation size (usually 256 bytes).  So round up to nearest
        // multiple of 256.  We do this by adding 255 and then masking off
        // the lower 2 bytes which store all bits < 256.
        // Example: Suppose byteSize = 300.
        // (300 + 255) & ~255
        // 555 & ~255
        // 0x022B & ~0x00ff
        // 0x022B & 0xff00
        // 0x0200
        // 512
        return (byteSize + 255) & ~255;
    }

    static Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename);

    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
        ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList,
        const void* initData,
        UINT64 byteSize,
        Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

    static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
        const std::wstring& filename,
        const D3D_SHADER_MACRO* defines,
        const std::string& entrypoint,
        const std::string& target);
};

class DxException
{
public:
    DxException() = default;
    DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

    std::wstring ToString()const;

    HRESULT ErrorCode = S_OK;
    std::wstring FunctionName;
    std::wstring Filename;
    int LineNumber = -1;
};




struct Vertex
{
	glm::vec3 Pos = glm::vec3(1.0f);
	glm::vec4 Color = glm::vec4(1.0f);
	glm::vec4 Normal = glm::vec4(1.0f);
	glm::vec2 TexCoord = glm::vec2(1.0f);
};

struct ConstantBuffer
{
    glm::mat4 LightTVP = glm::mat4(1.0f);
    glm::mat4 LightMVP = glm::mat4(1.0f);
    glm::mat4 World = glm::mat4(1.0f);
	glm::mat4 CameraMVP = glm::mat4(1.0f);
	glm::mat4 Scale3D = glm::mat4(1.0f);
	glm::mat4 Rotate = glm::mat4(1.0f);
	float Offset = 0;
};

struct Transform
{
	glm::vec4 Rotation = glm::vec4(1.0f);
	glm::vec3 Translation = glm::vec3(1.0f);
	glm::vec3 Scale3D = glm::vec3(1.0f);
};

struct MeshVertexInfo 
{
    std::vector<glm::vec3>  mVertex;
    std::vector<glm::vec4>  mNormal;
    std::vector<glm::vec4>  mTangent;
    std::vector<glm::vec2>  mTexCoord;
    std::vector<int>        mIndex;
};

struct MatData
{
	glm::vec4 BaseColor = glm::vec4(1.0f);
	glm::vec3 FresnelR0 = glm::vec3(1.0f);
	float Metallic = 0.05;
	float Specular = 0;
	float Roughness = 0.2;
};

struct LightData
{
	glm::vec3 LightDirection;
	float LightIntensity = 10.0f;
	glm::vec3 LightColor = glm::vec3(1.0f);
	float LightRadius = 3000.0f;
};

enum class HeapType
{
	CBV_SRV_UAV = 0,
	SAMPLER = 1,
	RTV = 2,
	DSV = 3,
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif