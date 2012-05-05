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

#include "GamePlayFramework.h"
#include "EngineManager.h"
#include "Plugin.h"
#include "boost/make_shared.hpp"
#include "Renderable.h"
#include "RenderMappingDefines.h"
#include "RenderSystem.h"

namespace GamePlay
{
	//---------------------------------------------------------------------
	GamePlayFramework::GamePlayFramework(const Util::Wstring & name)
	{
		Engine::EngineManager & engineMgr = Engine::EngineManager::getSingleton();
		engineMgr.setWindowName(name);

	}
	//---------------------------------------------------------------------
	GamePlayFramework::~GamePlayFramework()
	{}
	//---------------------------------------------------------------------
	void GamePlayFramework::run()
	{
		Engine::EngineManager & engineMgr = Engine::EngineManager::getSingleton();
		engineMgr.preRunning();
		init();
		engineMgr.run();
		engineMgr.postRunning();
	}
	//---------------------------------------------------------------------
	void GamePlayFramework::init()
	{
		/// Test for creating some content.
		struct Elem
		{
			float Pos[3];
		};
		
		Elem * elem = WHISPERWIND_NEW(Elem[6]);
		elem[0].Pos[0] = 0;
		elem[0].Pos[1] = 0;
		elem[0].Pos[2] = 1;
		elem[1].Pos[0] = 0;
		elem[1].Pos[1] = 1;
		elem[1].Pos[2] = 0;
		elem[2].Pos[0] = 1;
		elem[2].Pos[1] = 1;
		elem[2].Pos[2] = 1;
		elem[3].Pos[0] = 1;
		elem[3].Pos[1] = 1;
		elem[3].Pos[2] = 1;
		elem[4].Pos[0] = 1;
		elem[4].Pos[1] = 0;
		elem[4].Pos[2] = 1;
		elem[5].Pos[0] = 0;
		elem[5].Pos[1] = 1;
		elem[5].Pos[2] = 0;

		Engine::RenderableMappingPtr rm = boost::make_shared<Engine::RenderableMapping>();
		Engine::VertexElement vertexElem(0, 0, Engine::VET_FLOAT3, Engine::VEU_POSITION, 0);
		rm->VertexBound.VertexElemVec.push_back(vertexElem);
		rm->IndexBound.HasIndex = false;
		Engine::BufferData bufData;
		bufData.DataSize = sizeof(Elem) * 6;
		bufData.Data = static_cast<void *>(elem);
		bufData.Stride = sizeof(Elem);
		rm->VertexBound.VertexData = bufData;
		rm->VertexBound.VertexCount = 6;
		rm->VertexBound.VertexUsage = Engine::BUF_STATIC;
		rm->PrimCount = 2;
		rm->PrimType = Engine::PT_TRIANGLE_LIST;
		rm->EffectName = TO_UNICODE("Test.fx");
		rm->TechniqueName = "Test";

		Engine::RenderablePtr renderable = Engine::EngineManager::getSingleton().getRenderSystem()->createRenderable(rm);
		Engine::EngineManager::getSingleton().addRenderable(renderable);

		/// NOTE:Not exception-safe!
		WHISPERWIND_DELETE_ARRAY(elem); 
	}
}