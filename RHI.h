#pragma once
#include "RenderItem.h"

class RHI 
{
public:
	virtual ~RHI();

	virtual bool Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void BuildGeo() = 0;
	virtual void CreateRenderItem(std::vector<std::shared_ptr<RenderItem>> RI) = 0;

};

