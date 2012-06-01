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

#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

#include "CheckedCast.h"
#include "AABB.h"
#include "EngineManager.h"
#include "RenderSystem.h"
#include "RenderMappingDefines.h"
#include "Renderable.h"
#include "Camera.h"
#include "Frustum.h"
#include "DebugResource.h"
#include "DebugObject.h"

namespace Engine
{
	//---------------------------------------------------------------------
	// DebugObject
	//---------------------------------------------------------------------
	void DebugObject::addToRenderQueue()
	{
		BOOST_AUTO(it, mRenderableMap.begin());
		for (it; it != mRenderableMap.end(); ++it)
		{
			RenderablePtr & renderable = it->second;

			if (mAttachedSceneNode)
			{
				FrustumPtr & frustum = EngineManager::getSingleton().getCamera()->getFrustum();

				renderable->setWorldViewProj(frustum->getViewProjMatrix());
			}

			EngineManager::getSingleton().getRenderSystem()->addToRenderQueue(renderable);
		}
	}

	//---------------------------------------------------------------------
	// DebugFactory
	//---------------------------------------------------------------------
	static const Util::Wstring DEBUG_FACTORY_NAME(TO_UNICODE("debug"));
	//---------------------------------------------------------------------
	DebugFactory::DebugFactory() :
	    SceneObjectFactory(DEBUG_FACTORY_NAME)
	{}
	//---------------------------------------------------------------------
	SceneObjectPtr DebugFactory::create_impl(const Util::Wstring & objName, const ResourcePtr & resource)
	{
		DebugResourcePtr debugRes = Util::checkedPtrCast<DebugResource>(resource);
		const RenderableMappingPtr & rm = debugRes->getRenderableMapping();

		RenderablePtr renderable = EngineManager::getSingleton().getRenderSystem()->createRenderable(rm);

		DebugObjectPtr debugObj = boost::make_shared<DebugObject>(objName);
		debugObj->addRenderable(rm->RenderableName, renderable);

		return debugObj;
	}

}