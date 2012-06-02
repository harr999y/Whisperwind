/*-------------------------------------------------------------------------
This source file is a part of Whisperwind.(GameEngine + GamePlay + GameTools)
For the latest info, see http://lisuyong.com

Copyright (c) 2012 Suyong Li (79481268@qq.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE
-------------------------------------------------------------------------*/

#include "AABB.h"
#include "Renderable.h"
#include "EngineManager.h"
#include "Camera.h"
#include "Light.h"

namespace Engine
{
	//---------------------------------------------------------------------
	// Light
	//---------------------------------------------------------------------
	Light::Light(LightType lightType, const Util::Wstring & name) :
        SceneObject(name),
	    mType(lightType),
	    mColor(0.0f, 0.0f, 0.0f)
    {}
	//---------------------------------------------------------------------
	bool Light::isAffected(const Util::AABBPtr & aabb)
	{
		IF_NULL_RETURN_FALSE(aabb);

		return isAffected_impl(aabb);
	}
	//---------------------------------------------------------------------
	void Light::affectRenderable(const RenderablePtr & renderable)
	{
		/// Set light color.
		renderable->setEffectSemanticValue("LightColor", static_cast<void *>(&mColor));

		/// Set eye position.
		XMVECTOR eyePos(EngineManager::getSingleton().getCamera()->getPosition());
		renderable->setEffectSemanticValue("EyePosition", static_cast<void *>(&eyePos));

		affectRenderable_impl(renderable);
	}

	//---------------------------------------------------------------------
	// DirectionalLight
	//---------------------------------------------------------------------
	bool DirectionalLight::isAffected_impl(const Util::AABBPtr & /*aabb*/)
	{
		/// TODO!
		return true;
	}
	//---------------------------------------------------------------------
	void DirectionalLight::affectRenderable_impl(const RenderablePtr & renderable)
	{
		renderable->setEffectSemanticValue("LightDirection", static_cast<void *>(&mDirection));
	}

	//---------------------------------------------------------------------
	// PointLight
	//---------------------------------------------------------------------
	bool PointLight::isAffected_impl(const Util::AABBPtr & /*aabb*/)
	{
		/// TODO!
// 		if (mAttachedSceneNode && aabb &&
// 			(XMVectorGetX(XMVector3LengthEst(mAttachedSceneNode->getPosition() - aabb->getCenterPoint())) < mEffectDistance))
// 		{
// 			return true;
// 		}

		return true;
	}
	//---------------------------------------------------------------------
	void PointLight::affectRenderable_impl(const RenderablePtr & renderable)
	{
		if (mAttachedSceneNode && renderable->getAABB())
		{
			XMVECTOR dir = renderable->getAABB()->getCenterPoint() - mAttachedSceneNode->getPosition();

			renderable->setEffectSemanticValue("LightDirection", static_cast<void *>(&dir));
		}
	}

}