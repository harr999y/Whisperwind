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
#include "D3D9Plugin.h"
#include "EngineManager.h"
#include "D3D9RenderSystem.h"
#include "boost/make_shared.hpp"

namespace Engine
{
	//---------------------------------------------------------------------
	void D3D9Plugin::install()
	{
		EngineManager & engineMgr = EngineManager::getSingleton();
		RenderSystemPtr d3d9RS = boost::make_shared<D3D9RenderSystem>(engineMgr.getWindowName());

		d3d9RS->setEngineConfig(engineMgr.getEngineConfig());
		d3d9RS->init();

		engineMgr.setRenderSystem(d3d9RS);
	}
	//---------------------------------------------------------------------
	void D3D9Plugin::uninstall()
	{
 		EngineManager & engineMgr = EngineManager::getSingleton();
 		engineMgr.setRenderSystem(RenderSystemPtr());
	}
}