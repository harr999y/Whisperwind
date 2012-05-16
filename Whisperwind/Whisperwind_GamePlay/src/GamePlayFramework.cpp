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

#include <sstream>
#include <utility>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/make_shared.hpp>

#include "CheckedCast.h"
#include "MathDefine.h"
#include "EngineManager.h"
#include "Renderable.h"
#include "RenderMappingDefines.h"
#include "RenderTexture.h"
#include "RenderSystem.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "SceneNode.h"
#include "WindowsEventHandle.h"
#include "GamePlayForwardDeclare.h"
#include "Actor.h"
#include "Camera.h"
#include "GamePlayFramework.h"

namespace GamePlay
{
	//---------------------------------------------------------------------
	GamePlayFramework::GamePlayFramework(const Util::Wstring & name) :
		mRightMouseDown(false)
	{
		Engine::EngineManager & engineMgr = Engine::EngineManager::getSingleton();
		engineMgr.setWindowName(name);
	}
	//---------------------------------------------------------------------
	GamePlayFramework::~GamePlayFramework()
	{
		mKeyboard.reset();
		mMouse.reset();
		mInputManager.reset();
	}
	//---------------------------------------------------------------------
	void GamePlayFramework::run()
	{
		Engine::EngineManager & engineMgr = Engine::EngineManager::getSingleton();
		engineMgr.setup();

		engineMgr.getSceneManager()->regSceneObjectFactory(boost::make_shared<ActorFactory>());

		initInput();

		createScene();

		engineMgr.run();

		destroyScene();

		engineMgr.shutDown();
	}
	//---------------------------------------------------------------------
	void GamePlayFramework::initInput()
	{
		OIS::ParamList paramList;

		Util::u_int windowHandle;
		Engine::WindowsEventHandle::getWindow(&windowHandle);
		std::ostringstream windowHandleStr;
		windowHandleStr << windowHandle;
		paramList.insert(std::make_pair("WINDOW", windowHandleStr.str()));

		paramList.insert(std::make_pair(std::string("w32_mouse"), "DISCL_FOREGROUND"));
		paramList.insert(std::make_pair(std::string("w32_mouse"), "DISCL_NONEXCLUSIVE"));

		mInputManager = boost::shared_ptr<OIS::InputManager>(OIS::InputManager::createInputSystem(paramList),
		    boost::bind(&OIS::InputManager::destroyInputSystem, _1));

		mKeyboard = boost::shared_ptr<OIS::Keyboard>(static_cast<OIS::Keyboard *>(mInputManager->createInputObject(OIS::OISKeyboard, true)), 
			boost::bind(&OIS::InputManager::destroyInputObject, boost::ref(*(mInputManager.get())), _1));
		mMouse = boost::shared_ptr<OIS::Mouse>(static_cast<OIS::Mouse *>(mInputManager->createInputObject(OIS::OISMouse, true)), 
			boost::bind(&OIS::InputManager::destroyInputObject, boost::ref(*(mInputManager.get())), _1));

		mKeyboard->setEventCallback(this);
		mMouse->setEventCallback(this);
	}
	//---------------------------------------------------------------------
	void GamePlayFramework::createScene()
	{
		/// Test for creating some content.
		Engine::EngineManager & engineMgr = Engine::EngineManager::getSingleton();

		Engine::VoidDataPtr data(new (Util::real[20]));
		Util::real * elem = boost::static_pointer_cast<Util::real>(data).get();
		elem[0] = -1.25;
		elem[1] = -1;
		elem[2] = 1;
		elem[3] = 0;
		elem[4] = 1;
		elem[5] = -1.25;
		elem[6] = 1;
		elem[7] = 1;
		elem[8] = 0;
		elem[9] = 0;
		elem[10] = 1;
		elem[11] = -1;
		elem[12] = 1;
		elem[13] = 1;
		elem[14] = 1;
		elem[15] = 1;
		elem[16] = 1;
		elem[17] = 1;
		elem[18] = 1;
		elem[19] = 0;

		Engine::VoidDataPtr data2(new (Util::real[4]));
		Util::real * elem2 = boost::static_pointer_cast<Util::real>(data2).get();
		elem2[0] = 1.0;
		elem2[1] = 0.0;
		elem2[2] = 0.0;
		elem2[3] = 1.0;

		Engine::RenderableMappingPtr rm = boost::make_shared<Engine::RenderableMapping>();
		Engine::VertexElement vertexElem(0, 0, Engine::VET_FLOAT3, Engine::VEU_POSITION, 0);
		Engine::VertexElement vertexElem2(0, 3 * 4, Engine::VET_FLOAT2, Engine::VEU_TEXTURE_COORD, 0);
		Engine::VertexElement vertexElem3(1, 0, Engine::VET_FLOAT4, Engine::VEU_TEXTURE_COORD, 1);
		rm->VertexBound.VertexElemVec.push_back(vertexElem);
		rm->VertexBound.VertexElemVec.push_back(vertexElem2);
		rm->VertexBound.VertexElemVec.push_back(vertexElem3);

		Engine::BufferData bufData;
		bufData.DataSize = sizeof(Util::real) * 20;
		bufData.Data = data;
		bufData.Stride = sizeof(Util::real) * 5;
		rm->VertexBound.VertexDataVec.push_back(bufData);

		Engine::BufferData bufData2;
		bufData2.DataSize = sizeof(Util::real) * 4;
		bufData2.Data = data2;
		bufData2.Stride = 0;
		rm->VertexBound.VertexDataVec.push_back(bufData2);

		rm->VertexBound.VertexCount = 4;
		rm->VertexBound.VertexUsage = Engine::BUF_STATIC;
		
		rm->IndexBound.HasIndex = true;
 		Engine::VoidDataPtr data3(new (Util::u_int16[4]));
 		Util::u_int16 * elem3 = boost::static_pointer_cast<Util::u_int16>(data3).get();
 		elem3[0] = 0;
 		elem3[1] = 1;
 		elem3[2] = 2;
 		elem3[3] = 3;
 	
		rm->IndexBound.IndexData.Data = data3;
		rm->IndexBound.IndexData.DataSize = sizeof(Util::u_int16) * 4;
		rm->IndexBound.IndexData.Stride = sizeof(Util::u_int16);
		rm->IndexBound.IndexFmt = Engine::INDEX_16;
		rm->IndexBound.IndexUsage = Engine::BUF_STATIC;

		rm->PrimType = Engine::PT_TRIANGLE_STRIP;
		rm->EffectName = engineMgr.getResourceManager()->getResourcePath(TO_UNICODE("Test.fx"));
		rm->TechniqueName = "Test";

		Engine::RenderablePtr renderable = Engine::EngineManager::getSingleton().getRenderSystem()->createRenderable(rm);

		Util::Wstring actorName(TO_UNICODE("test"));
		mActor = Util::checkedPtrCast<Actor>(engineMgr.getSceneManager()->createSceneObject(TO_UNICODE("Actor"), actorName));
		mActor->setRenderable(renderable);
		renderable->regPostRenderCallback(boost::bind(&GamePlayFramework::preUpdateCallback, boost::ref(*this), _1));

		Engine::SceneNodePtr node = Engine::EngineManager::getSingleton().getSceneManager()->createSceneNode(actorName, Engine::NT_STATIC);
		node->setPosition(XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f));
		node->attachSceneObject(mActor);

		Util::Wstring texturePath(engineMgr.getResourceManager()->getResourcePath(TO_UNICODE("test.dds")));
		mRenderTexture = Engine::EngineManager::getSingleton().getRenderSystem()->createRenderTextureFromFile(texturePath);

		mCamera = boost::make_shared<Camera>(0.0f, 2000.0f);
		mCamera->setPosition(XMFLOAT3(0.0, 0.0, 0.0));
		mCamera->lookAt(XMVectorSet(0.0, 0.0, 1.0, 0.0));

		engineMgr.getSceneManager()->regPreUpdateCallback(boost::bind(&GamePlayFramework::preUpdate, boost::ref(*this), _1));
	}
	//---------------------------------------------------------------------
	void GamePlayFramework::preUpdateCallback(Util::time elapsedTime)
	{
		Engine::RenderablePtr renderable;
		renderable = mActor->getRenderable();
		IF_NULL_RETURN(renderable);

 		static Util::real num = 0.0f;
 		num += 1.f * elapsedTime;
 		Util::real test[4] = {num, num, num, 1.0f};
		renderable->setEffectSemanticValue("COLOR3", static_cast<void *>(test));

		renderable->setTexture("tex", mRenderTexture);

		XMMATRIX matrix = XMMatrixIdentity();
		Engine::SceneNodePtr & node = mActor->getAttachedSceneNode();
		if (node)
		{
			XMVECTOR pos = node->getPosition();
			//pos -= XMVectorSet(0.0f, 0.0f, 3.1f, 0.0f) * elapsedTime;
			node->setPosition(pos);
			matrix = XMMatrixTranslationFromVector(pos);
		}

		matrix *= mCamera->getViewMatrix() * mCamera->getProjMatrix();
		renderable->setEffectSemanticValue("WORLDVIEWPROJ", static_cast<void *>(&matrix));
	}
	//---------------------------------------------------------------------
	bool GamePlayFramework::keyPressed(const OIS::KeyEvent & arg)
	{
		Util::u_int moveDirection = mCamera->getKeyCombinationFromEvent(arg);

		mCamera->move(moveDirection);

		return true;
	}
	//---------------------------------------------------------------------
	bool GamePlayFramework::keyReleased(const OIS::KeyEvent & arg)
	{
		Util::u_int unmoveDirection = mCamera->getKeyCombinationFromEvent(arg);

		mCamera->stopMove(unmoveDirection);

		return true;
	}
	//---------------------------------------------------------------------
	bool GamePlayFramework::mouseMoved(const OIS::MouseEvent & arg)
	{
		if (mRightMouseDown)
			mCamera->rotate(static_cast<Util::real>(arg.state.Y.rel), static_cast<Util::real>(arg.state.X.rel));

		return true;
	}
	//---------------------------------------------------------------------
	bool GamePlayFramework::mousePressed(const OIS::MouseEvent & /*arg*/, OIS::MouseButtonID id)
	{
		if (id == OIS::MB_Right)
			mRightMouseDown = true;

		return true;
	}
	//---------------------------------------------------------------------
	bool GamePlayFramework::mouseReleased(const OIS::MouseEvent & /*arg*/, OIS::MouseButtonID id)
	{
		if (id == OIS::MB_Right)
			mRightMouseDown = false;

		return true;
	}
	//---------------------------------------------------------------------
	void GamePlayFramework::preUpdate(Util::time elapsedTime)
	{
		if (mMouse)
			mMouse->capture();

		if (mKeyboard)
			mKeyboard->capture();

		mCamera->update(elapsedTime);
	}
	//---------------------------------------------------------------------
	void postUpdate(Util::time /*elapsedTime*/)
	{}
	//---------------------------------------------------------------------
	void GamePlayFramework::destroyScene()
	{}

 }