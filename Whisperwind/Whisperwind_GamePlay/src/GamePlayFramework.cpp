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
#include "boost/make_shared.hpp"
#include "Renderable.h"
#include "RenderMappingDefines.h"
#include "RenderSystem.h"
#include "Actor.h"
#include "GamePlayForwardDeclare.h"
#include "SceneManager.h"
#include "SceneNode.h"
#include "SceneObject.h"
#include "CheckedCast.h"

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
		engineMgr.setup();

		createScene();

		engineMgr.run();

		destroyScene();

		engineMgr.shutDown();
	}
	//---------------------------------------------------------------------
	void GamePlayFramework::createScene()
	{
		/// Test for creating some content.
		Engine::VoidDataPtr data(new (Util::real[18]));
		Util::real * elem = boost::static_pointer_cast<Util::real>(data).get();
		elem[0] = 0;
		elem[1] = 0;
		elem[2] = 1;
		elem[3] = 0;
		elem[4] = 1;
		elem[5] = 0;
		elem[6] = 1;
		elem[7] = 1;
		elem[8] = 1;
		elem[9] = 1;
		elem[10] = 1;
		elem[11] = 1;
		elem[12] = 1;
		elem[13] = 0;
		elem[14] = 1;
		elem[15] = 0;
		elem[16] = 1;
		elem[17] = 0;

		Engine::RenderableMappingPtr rm = boost::make_shared<Engine::RenderableMapping>();
		Engine::VertexElement vertexElem(0, 0, Engine::VET_FLOAT3, Engine::VEU_POSITION, 0);
		rm->VertexBound.VertexElemVec.push_back(vertexElem);
		rm->IndexBound.HasIndex = false;
		Engine::BufferData bufData;
		bufData.DataSize = sizeof(Util::real) * 18;
		bufData.Data = data;
		bufData.Stride = sizeof(Util::real) * 3;
		rm->VertexBound.VertexData = bufData;
		rm->VertexBound.VertexCount = 6;
		rm->VertexBound.VertexUsage = Engine::BUF_STATIC;
		rm->PrimCount = 2;
		rm->PrimType = Engine::PT_TRIANGLE_LIST;
		rm->EffectName = TO_UNICODE("Test.fx");
		rm->TechniqueName = "Test";

		Engine::RenderablePtr renderable = Engine::EngineManager::getSingleton().getRenderSystem()->createRenderable(rm);

		Util::Wstring actorName(TO_UNICODE("test"));
		ActorPtr actor = boost::make_shared<Actor>(actorName);
		actor->regComponent(Engine::CT_RENDERABLE, renderable);

		Engine::SceneNodePtr node = Engine::EngineManager::getSingleton().getSceneManager()->createSceneNode(actorName);
		Engine::SceneObjectPtr obj = Util::checkedPtrCast<Engine::SceneObject>(actor);
		node->attachSceneObject(obj);
		Engine::EngineManager::getSingleton().getSceneManager()->getRootNode()->addChildNode(node);
	}
	//---------------------------------------------------------------------
	void GamePlayFramework::destroyScene()
	{

	}

 }