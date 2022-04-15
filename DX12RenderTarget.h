#pragma once
#include "RenderTarget.h"
#include "UpLoadBuffer.h"

class DX12RenderTarget :
    public RenderTarget
{
public:
	DX12RenderTarget() {}
	DX12RenderTarget(std::string name, UINT32 width, UINT32 height, int num, RtType type) : RenderTarget(name, width, height, num, type) {}
	
	void SetViewportAndRect(D3D12_VIEWPORT v, D3D12_RECT r) { mViewport = v; mScissorRect = r; }
	void SetRTBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> ryb);
	void SetDSBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> dsb);

	Microsoft::WRL::ComPtr<ID3D12Resource>& GetRTBuffer() { return mRTBuffers; }
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetDSBuffer() { return mDSBuffer; }

	void AddOffset(std::string name, int offset); 
	UINT32 GetOffset(std::string name);

	Microsoft::WRL::ComPtr<ID3D12Resource>& GetBuffer(RTBufferType type);

	D3D12_VIEWPORT GetViewPort() { return mViewport; }
	D3D12_RECT GetRect() { return mScissorRect; }


	
protected:

	Microsoft::WRL::ComPtr<ID3D12Resource> mRTBuffers = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> mDSBuffer = nullptr;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	

	std::unordered_map<std::string, UINT32> OffsetMap;
};

