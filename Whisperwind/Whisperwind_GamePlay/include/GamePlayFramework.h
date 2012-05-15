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
#ifndef _GAMEPLAY_FRAMEWORK_H_
#define _GAMEPLAY_FRAMEWORK_H_

#pragma warning(push, 3)
#include <OIS/includes/OIS.h>
#pragma warning(pop)

#include "Util.h"
/// TODO!
#include "EngineForwardDeclare.h"
#include "SceneComponent.h"
#include "GamePlayForwardDeclare.h"

namespace GamePlay
{
	class WHISPERWIND_API GamePlayFramework : public OIS::KeyListener, public OIS::MouseListener
	{
	public:
		explicit GamePlayFramework(const Util::Wstring & name);
		~GamePlayFramework();

	public:
		void run();

		/// TODO!
		void preUpdateCallback(Util::time elapsedTime);
		void preUpdate(Util::time elapsedTime);
		void postUpdate(Util::time elapsedTime);

		virtual bool keyPressed(const OIS::KeyEvent & arg);
		virtual bool keyReleased(const OIS::KeyEvent & arg);
		virtual bool mouseMoved(const OIS::MouseEvent & arg);
		virtual bool mousePressed(const OIS::MouseEvent & arg, OIS::MouseButtonID id);
		virtual bool mouseReleased(const OIS::MouseEvent & arg, OIS::MouseButtonID id);

	private:
		void initInput();
		void createScene();
		void destroyScene();

	private:
		ActorPtr mActor;
		Engine::RenderTexturePtr mRenderTexture;
		CameraPtr mCamera;

		typedef boost::shared_ptr<OIS::InputManager> InputManagerPtr;
		typedef boost::shared_ptr<OIS::Keyboard> KeyboardPtr;
		typedef boost::shared_ptr<OIS::Mouse> MousePtr;
 		InputManagerPtr mInputManager;
 		KeyboardPtr mKeyboard;
 		MousePtr mMouse;

		bool mRightMouseDown;

	private:
		DISALLOW_COPY_AND_ASSIGN(GamePlayFramework);
	};
}

#endif