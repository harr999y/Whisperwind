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

#include "EngineManager.h"
#include "EngineConfig.h"
#include "RenderMappingDefines.h"
#include "RenderQueue.h"
#include "Renderable.h"
#include "Viewport.h"
#include "RenderSystem.h"

namespace Engine
{
	//---------------------------------------------------------------------
	RenderSystem::~RenderSystem()
	{
		mOpaqueRenderQueue.reset();
		mTransparentRenderQueue.reset();	
	}
	//---------------------------------------------------------------------
	void RenderSystem::init()
	{
		mEngineConfig = EngineManager::getSingleton().getEngineConfig();

		init_impl();

		if (mEngineConfig->getDefferdRendering())
			mOpaqueRenderQueue = boost::make_shared<DeferredRenderQueue>();
		else
			mOpaqueRenderQueue = boost::make_shared<ForwardRenderQueue>();

		mTransparentRenderQueue = boost::make_shared<ForwardRenderQueue>();
	}
	//---------------------------------------------------------------------
	bool RenderSystem::beginRendering()
	{
		return (beginRendering_impl());
	}
	//---------------------------------------------------------------------
	bool RenderSystem::render(const RenderablePtr & renderable)
	{
		return render_impl(renderable);
	}
	//---------------------------------------------------------------------
	void RenderSystem::endRendering()
	{
		endRendering_impl();
	}
	//---------------------------------------------------------------------
	void RenderSystem::present()
	{
		present_impl();
	}
	//---------------------------------------------------------------------
	void RenderSystem::clearFrame(Util::u_int flag, Util::real zValue, Util::u_int stencilValue)
	{
		clearFrame_impl(flag, zValue, stencilValue);
	}
	//---------------------------------------------------------------------
	bool RenderSystem::isPaused()
	{
		return isPaused_impl();
	}
	//---------------------------------------------------------------------
	RenderablePtr RenderSystem::createRenderable(const RenderableMappingPtr & rm)
	{
		return createRenderable_impl(rm);
	}
	//---------------------------------------------------------------------
	Engine::RenderTexturePtr RenderSystem::createRenderTexture( const RenderTextureMappingPtr & rtm )
	{
		return createRenderTexture_impl(rtm);
	}
	//---------------------------------------------------------------------
	RenderTexturePtr RenderSystem::createRenderTextureFromFile(const Util::Wstring & path)
	{
		return createRenderTextureFromFile_impl(path);
	}
	//---------------------------------------------------------------------
	RenderTargetPtr RenderSystem::createRenderTarget(const RenderTargetMappingPtr & rtm)
	{
		return createRenderTarget_impl(rtm);
	}
	//---------------------------------------------------------------------
	void RenderSystem::setRenderTarget(Util::u_int index, const RenderTargetPtr & target)
	{
		setRenderTarget_impl(index, target);
	}
	//---------------------------------------------------------------------
	void RenderSystem::setRenderTarget(Util::u_int index, const RenderTexturePtr & texture)
	{
		setRenderTarget_impl(index, texture);
	}
	//---------------------------------------------------------------------
	void RenderSystem::setBlendFactor(BlendFactor srcFactor, BlendFactor destFactor)
	{
		setBlendFactor_impl(srcFactor, destFactor);
	}
	//---------------------------------------------------------------------
	void RenderSystem::closeBlend()
	{
		closeBlend_impl();
	}
	//---------------------------------------------------------------------
	void RenderSystem::addToRenderQueue(const RenderablePtr & renderable)
	{
		if (RT_OPAQUE == renderable->getRenderType())
			mOpaqueRenderQueue->addRenderable(renderable);
		else
			mTransparentRenderQueue->addRenderable(renderable);
	}
	//---------------------------------------------------------------------
	void RenderSystem::renderScene(Util::time elapsedTime)
	{
		this->clearFrame(FCF_TARGET | FCF_ZBUFFER);

		IF_FALSE_RETURN(this->beginRendering());

		mOpaqueRenderQueue->render(elapsedTime);

		mTransparentRenderQueue->render(elapsedTime);

		this->endRendering();
	}

}