#pragma once

#include "Scene.h"
#include "GameTimer.h"
#include "SceneRender.h"


/// <summary>  TEST
enum class RtType
{
	BaseRt,
	ShadowRt,
};
/// </summary>

class RHI 
{
public:
	virtual ~RHI();

	virtual bool Init() = 0;


	
	

	
	
	virtual void ChangeResState() = 0;
	virtual void CloseRtv() = 0;

	virtual void BeginDrawShadow() = 0;

	virtual void SetPSO(Pipeline* pl) = 0;


	virtual Texture* CreateTexture(Texture* t) = 0;
	/*virtual Material* CreateMaterial(Material* m) = 0;*/
	virtual Mesh* CreateMesh(Mesh* m) = 0;
	virtual Pipeline* CreatePipeline(Pipeline* p) = 0;
	virtual Shader* CreateShader(Shader* s) = 0;
	virtual Primitive* CreatePrimitive(Primitive* p) = 0;

	//some test
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void InputAssetInfo(Mesh* mesh) = 0;
	virtual void SetRootSignature(Shader* s) = 0;
	virtual void OpenCmdList() = 0;
	virtual void CloseCmdList() = 0;

	//±ØÐë¸ÄµÄ
	virtual void SetRTVAndDSV(RtType rt) = 0;
	virtual void ResetViewportsAndScissorRects(RtType rt) = 0;

	virtual void BindDataTable(UINT32 Slot, UINT32 HandleOffset, HeapType ht) = 0;
	virtual void BindDataConstantBuffer(UINT32 Slot, UINT32 Address) = 0;
	virtual void Bind32BitConstants(UINT32 Slot, UINT32 num, const void* data, UINT32 offset) = 0;
	virtual void DrawMesh(UINT32 IndexCount) = 0;

};

