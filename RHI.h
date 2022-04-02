#pragma once

#include "Scene.h"
#include "GameTimer.h"
#include "SceneRender.h"

class RHI 
{
public:
	virtual ~RHI();

	virtual bool Init() = 0;
	virtual void UpdateTrans(Scene* mScene, Primitive* actor) = 0;
	virtual void DrawInstance(Primitive* actor) = 0;
	virtual void BuildGeo(Primitive* actor) = 0;
	
	virtual void ResetViewportsAndScissorRects() = 0;
	virtual void ClearRTVAndDSV() = 0;
	virtual void SetRTVAndDSV() = 0;
	virtual void SetRootSignature() = 0;
	virtual void OpenRtv() = 0;
	virtual void CloseRtv() = 0;
	virtual void DrawItemShadow(Primitive* actor) = 0;
	virtual void UpdateLight(const GameTimer& gt) = 0;
	virtual void BeginDrawShadow() = 0;

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
};

