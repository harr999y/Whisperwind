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

#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/make_shared.hpp>

#include "Profile.h"
#include "EngineManager.h"
#include "SceneManager.h"
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

		PROFILE(createScene(), TO_UNICODE("create scene"));

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
			boost::bind(&OIS::InputManager::destroyInputObject, boost::ref(*mInputManager), _1));
		mMouse = boost::shared_ptr<OIS::Mouse>(static_cast<OIS::Mouse *>(mInputManager->createInputObject(OIS::OISMouse, true)), 
			boost::bind(&OIS::InputManager::destroyInputObject, boost::ref(*mInputManager), _1));

		mKeyboard->setEventCallback(this);
		mMouse->setEventCallback(this);
	}
	//---------------------------------------------------------------------
	void GamePlayFramework::createScene()
	{
 		Engine::SceneManagerPtr & sceneMgr = Engine::EngineManager::getSingleton().getSceneManager();
 
 		sceneMgr->loadScene(TO_UNICODE("test.wscene"));
 
 		sceneMgr->regPreUpdateCallback(boost::bind(&GamePlayFramework::preUpdate, boost::ref(*this), _1));

		mCamera = Engine::EngineManager::getSingleton().getCamera();
	}
	//---------------------------------------------------------------------
	bool GamePlayFramework::keyPressed(const OIS::KeyEvent & arg)
	{
		Util::u_int moveDirection = getKeyCombinationFromEvent(arg);

		mCamera->move(moveDirection);

		return true;
	}
	//---------------------------------------------------------------------
	bool GamePlayFramework::keyReleased(const OIS::KeyEvent & arg)
	{
		Util::u_int unmoveDirection = getKeyCombinationFromEvent(arg);

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
	//---------------------------------------------------------------------
	Util::u_int GamePlayFramework::getKeyCombinationFromEvent(const OIS::KeyEvent & arg) const
	{
		Util::u_int flag = 0;
		if (OIS::KC_W == arg.key)
			flag |= Engine::MD_FORWARD;
		else if (OIS::KC_S == arg.key)
			flag |= Engine::MD_BACK;
		else if (OIS::KC_A == arg.key)
			flag |= Engine::MD_LEFT;
		else if (OIS::KC_D == arg.key)
			flag |= Engine::MD_RIGHT;

		return flag;
	}

 }