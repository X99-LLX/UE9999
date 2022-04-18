#pragma once

#include "Scene.h"
#include "GameTimer.h"
#include "SceneRender.h"




class RHI 
{
public:
	virtual ~RHI();
	virtual bool Init() = 0;
	//virtual void ChangeResState() = 0;
	virtual void ChangeResState(BufferState CurrnetState, BufferState ChangeState, RenderTarget* rt, RTBufferType type) = 0;
	virtual void SetPSO(Pipeline* pl) = 0;
	virtual std::shared_ptr<Texture> CreateTexture(Texture* t) = 0;
	virtual std::shared_ptr<Material> CreateMaterial(Material* m) = 0;
	virtual std::shared_ptr<Mesh> CreateMesh(std::shared_ptr<Mesh> m) = 0;
	virtual std::shared_ptr<Pipeline> CreatePipeline(Pipeline* p) = 0;
	virtual std::shared_ptr<Shader> CreateShader(Shader* s) = 0;
	virtual std::shared_ptr<Primitive> CreatePrimitive(Primitive* p) = 0;
	virtual std::shared_ptr<RenderTarget> CreateRenderTarget(std::string name, UINT32 width, UINT32 height, int num, RtType type) = 0;
	virtual std::shared_ptr<Light> CreateLight() = 0;

	virtual void UpdateRenderTarget(std::shared_ptr<RenderTarget> rt, ColorFormat format) = 0;

	virtual void ClearRenderTarget(RenderTarget* rt) = 0;
	virtual void ClearDepthStencil(RenderTarget* rt) = 0;
	virtual void EventBegin(std::string EventName) = 0;
	virtual void EventEnd() = 0;

	//some test

	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void InputAssetInfo(Mesh* mesh) = 0;
	virtual void SetRootSignature(Shader* s) = 0;
	virtual void OpenCmdList() = 0;
	virtual void CloseCmdList() = 0;

	//±ØÐë¸ÄµÄ
	virtual void SetRTVAndDSV(RenderTarget* rt) = 0;
	virtual void ResetViewportsAndScissorRects(RenderTarget* rt) = 0;

	virtual void BindDataTable(UINT32 Slot, UINT32 HandleOffset, HeapType ht) = 0;
	virtual void BindDataConstantBuffer(UINT32 Slot, UINT32 Address) = 0;
	virtual void Bind32BitConstants(UINT32 Slot, UINT32 num, const void* data, UINT32 offset) = 0;
	virtual void DrawMesh(UINT32 IndexCount) = 0;
	virtual void ReSetCmdAlloc() = 0;

};

