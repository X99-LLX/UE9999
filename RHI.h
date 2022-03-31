#pragma once

#include "Scene.h"
#include "GameTimer.h"

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
	virtual void Swapchain() = 0;
	virtual void DrawItemShadow(Primitive* actor) = 0;
	virtual void UpdateLight(const GameTimer& gt) = 0;
	virtual void BeginDrawShadow() = 0;


	//some test
	virtual void InputAssetInfo(Mesh* mesh) = 0;
};

