/*-------------------------------------------------------------------------
This source file is a part of Whisperwind(GameEngine + GamePlay + GameTools)
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
#ifndef _ENGINE_NAMAGER_H_
#define _ENGINE_NAMAGER_H_

#include "Util.h"
#include <vector>

#include "EngineForwardDeclare.h"

namespace Engine
{
	class WHISPERWIND_API EngineManager
	{
	public:
		EngineManager();
		~EngineManager()
		{}

	public:
		static EngineManager & getSingleton();

		void preRunning();
		void run();
		void postRunning();

		void installPlugin(const Util::PluginPtr & plugin);

	public:
		SET_GET_CONST_VALUE(Util::wstring, WindowName);
		SET_GET_VALUE(RenderSystemPtr, RenderSystem);
		SET_GET_CONST_VALUE(bool, QuitLooping);

	private:
		void loadPlugins();
		void loadResources();

		void handleGamePlayLogical();

		void clearResources();
		void clearPlugins();

		DISALLOW_COPY_AND_ASSIGN(EngineManager);

	private:
		bool mQuitLooping;
		Util::wstring mWindowName;

		typedef std::vector<Util::PluginPtr> PluginVector;
		PluginVector mPluginVector;

		RenderSystemPtr mRenderSystem;

		static EngineManager mSingleton;
	};
}

#endif