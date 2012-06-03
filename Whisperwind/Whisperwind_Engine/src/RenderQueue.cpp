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

#include <boost/foreach.hpp>

#include "EngineManager.h"
#include "SceneManager.h"
#include "RenderSystem.h"
#include "Renderable.h"
#include "Light.h"
#include "RenderQueue.h"

namespace Engine
{
	//---------------------------------------------------------------------
	void RenderQueue::addRenderable(const RenderablePtr & renderable)
	{
		mRenderableVec.push_back(renderable);
	}
	//---------------------------------------------------------------------
	void RenderQueue::render(Util::time elapsedTime)
	{
		BOOST_FOREACH(const RenderablePtr & renderable, mRenderableVec)
		{
			renderable->preRender(elapsedTime);

			/// For forward lighting.
			const LightVector & lightVec = EngineManager::getSingleton().getSceneManager()->getAffectedLights(renderable->getAABB());
			if (lightVec.empty())
			{
				EngineManager::getSingleton().getRenderSystem()->render(renderable);
			}
			else
			{
				for (Util::u_int lightIt = 0; lightIt < lightVec.size(); ++lightIt)
				{
					lightVec[lightIt]->affectRenderable(renderable);

					if (lightIt >= 1)
						renderable->setBlendFactor(BF_ONE, BF_ONE);

					EngineManager::getSingleton().getRenderSystem()->render(renderable);
				}

				if (lightVec.size() >=1)
					renderable->closeBlend();
			}

			renderable->postRender(elapsedTime);
		}

		mRenderableVec.clear();
	}
	//---------------------------------------------------------------------
	void RenderQueue::sort()
	{
		/// TODO!
	}

}