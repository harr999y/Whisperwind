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
#include <boost/make_shared.hpp>

#include "MathDefine.h"
#include "EngineManager.h"
#include "EngineConfig.h"
#include "SceneManager.h"
#include "RenderSystem.h"
#include "Renderable.h"
#include "Light.h"
#include "RenderMappingHelper.h"
#include "RenderTexture.h"
#include "Camera.h"
#include "Frustum.h"
#include "RenderQueue.h"

namespace Engine
{
	//---------------------------------------------------------------------
	// RenderQueue
	//---------------------------------------------------------------------
	void RenderQueue::addRenderable(const RenderablePtr & renderable)
	{
		mRenderableVec.push_back(renderable);
	}
	//---------------------------------------------------------------------
	void RenderQueue::render(Util::time elapsedTime)
	{
		render_impl(elapsedTime);
	}
	//---------------------------------------------------------------------
	void RenderQueue::sort()
	{
		/// TODO!
	}

	//---------------------------------------------------------------------
	// ForwardRenderQueue
	//---------------------------------------------------------------------
	void ForwardRenderQueue::render_impl(Util::time elapsedTime)
	{
		IF_FALSE_RETURN(!mRenderableVec.empty());

		const RenderSystemPtr & rs = EngineManager::getSingleton().getRenderSystem();

		BOOST_FOREACH(const RenderablePtr & renderable, mRenderableVec)
		{
			renderable->preRender(elapsedTime);

			const LightVector & lightVec = EngineManager::getSingleton().getSceneManager()->getAffectedLights(renderable->getAABB());
			if (lightVec.empty())
			{
				rs->render(renderable);
			}
			else
			{
				for (Util::u_int lightIt = 0; lightIt < lightVec.size(); ++lightIt)
				{
					lightVec[lightIt]->affectRenderable(renderable);

					if (lightIt >= 1)
						rs->setBlendFactor(BF_ONE, BF_ONE);

					rs->render(renderable);
				}

				if (lightVec.size() >=1)
					rs->closeBlend();
			}

			renderable->postRender(elapsedTime);
		}

		mRenderableVec.clear();
	}

	//---------------------------------------------------------------------
	// DeferredRenderQueue
	//---------------------------------------------------------------------
	static const Util::String DEFERRED_RENDERING_EFFECT("DefferdLighting.fx");
	//---------------------------------------------------------------------
	DeferredRenderQueue::DeferredRenderQueue()
	{
		const EngineConfigPtr & engineConfig = EngineManager::getSingleton().getEngineConfig();
		RenderTextureMappingPtr texMapping = boost::make_shared<RenderTextureMapping>();
		texMapping->Width = engineConfig->getResolutionPair().first;
		texMapping->Height = engineConfig->getResolutionPair().second;
		texMapping->Usage = TCF_RENDERTARGET;
		texMapping->Format = RPF_A16B16G16R16F;

		/// GBuffer
		mGBufferTexture = EngineManager::getSingleton().getRenderSystem()->createRenderTexture(texMapping);

		/// LightingPass
		mLightingPassTexture = EngineManager::getSingleton().getRenderSystem()->createRenderTexture(texMapping);

		constructScreenQuadRenderable();
	}
	//---------------------------------------------------------------------
	DeferredRenderQueue::~DeferredRenderQueue()
	{
		mScreenQuadRenderable.reset();
		mGBufferTexture.reset();
		mLightingPassTexture.reset();
	}
	//---------------------------------------------------------------------
	void DeferredRenderQueue::render_impl(Util::time elapsedTime)
	{
		IF_FALSE_RETURN(!mRenderableVec.empty());

		const RenderSystemPtr & rs = EngineManager::getSingleton().getRenderSystem();

		/// 1.Construct GBuffer.
 		{
 			rs->setRenderTarget(0, mGBufferTexture);
 			rs->clearFrame(FCF_TARGET | FCF_ZBUFFER);
 
 			BOOST_FOREACH(const RenderablePtr & renderable, mRenderableVec)
 			{
 				renderable->setEffect(DEFERRED_RENDERING_EFFECT);
 				renderable->setTechnique("StoreGBuffer");
 
 				renderable->preRender(elapsedTime);
 
 				rs->render(renderable);
 
 				renderable->postRender(elapsedTime);
 			}
 			rs->setRenderTarget(0, RenderTexture::getNullRenderTexture());
 		}

		/// 2.Lighting pass.
 		{
 			rs->setRenderTarget(0, mLightingPassTexture);
 			rs->clearFrame(FCF_TARGET | FCF_ZBUFFER);
 
			XMMATRIX viewMatrix = EngineManager::getSingleton().getCamera()->getFrustum()->getViewMatrix();
			XMVECTOR determinant = XMMatrixDeterminant(viewMatrix);
			XMMATRIX inverseViewMatrix = XMMatrixInverse(&determinant, viewMatrix);

			const Util::UintPair & res = EngineManager::getSingleton().getEngineConfig()->getResolutionPair();
			XMVECTOR resolution = XMVectorSet(static_cast<Util::real>(res.first), static_cast<Util::real>(res.second), 0.0f, 0.0f);

 			/// TODO:Now do a hack to find affected lights.
 			const LightVector & lightVec = EngineManager::getSingleton().getSceneManager()->getAffectedLights(mRenderableVec[0]->getAABB());
 			BOOST_FOREACH(const LightPtr & light, lightVec)
 			{
 				mScreenQuadRenderable->preRender(elapsedTime);
 
 				light->affectRenderable(mScreenQuadRenderable);
 
				mScreenQuadRenderable->setTexture("lighting_pass_texture", mGBufferTexture);
				mScreenQuadRenderable->setEffectSemanticValue("InverseView", &inverseViewMatrix);
				mScreenQuadRenderable->setEffectSemanticValue("Resolution", &resolution);

 				rs->render(mScreenQuadRenderable);
 
 				mScreenQuadRenderable->postRender(elapsedTime);
 			}
 			rs->setRenderTarget(0, RenderTexture::getNullRenderTexture());
 		}

		/// 3.Shading pass.
		{
			rs->setRenderTarget(0, RenderTexture::getNullRenderTexture());

			BOOST_FOREACH(const RenderablePtr & renderable, mRenderableVec)
			{
				renderable->setEffect(DEFERRED_RENDERING_EFFECT);
				renderable->setTechnique("ShadingPass");

				renderable->preRender(elapsedTime);

				renderable->setTexture("lighting_pass_texture", mLightingPassTexture);

				rs->render(renderable);

				renderable->postRender(elapsedTime);
			}
		}

		mRenderableVec.clear();
	}
	//---------------------------------------------------------------------
	void DeferredRenderQueue::constructScreenQuadRenderable()
	{
		RenderableMappingPtr rm = RenderMappingHelper::makeScreenQuadRenderMapping();

		rm->EffectName = DEFERRED_RENDERING_EFFECT;
		rm->TechniqueName = "LightingPass";
		rm->RenderableName = TO_UNICODE("LightPassRenderable");

		mScreenQuadRenderable = EngineManager::getSingleton().getRenderSystem()->createRenderable(rm);
	}

}