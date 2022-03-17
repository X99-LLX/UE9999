#include "stdafx.h"
#include "Actor.h"
#include "Engine.h"
#include "ResourceManage.h"

Actor::Actor()
{

}

Actor::~Actor()
{

}

void Actor::ChangePosition(glm::vec3 Pos)
{
	Trans.Translation = Pos;
}

void Actor::ChangeRotation(glm::vec4 Rot)
{
	Trans.Rotation = Rot;
}

void Actor::ChangeScale3D(glm::vec3 Sca)
{
	Trans.Scale3D = Sca;
}

