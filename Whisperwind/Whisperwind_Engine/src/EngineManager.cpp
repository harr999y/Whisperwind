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

#include "EngineManager.h"
#include "Plugin.h"
#include "RenderSystem.h"
#include "WindowsEventHandle.h"
#include "boost/foreach.hpp"

namespace Engine
{
	//---------------------------------------------------------------------
	EngineManager EngineManager::mSingleton;
	EngineManager & EngineManager::getSingleton()
	{
		return mSingleton;
	}
	//---------------------------------------------------------------------
	EngineManager::EngineManager()
	{
		setQuitLooping(false);
		setWindowName(Util::BLANK_WSTRING);
	}
	//---------------------------------------------------------------------
	void EngineManager::installPlugin(const Util::PluginPtr & plugin)
	{
		plugin->install();

		mPluginVector.push_back(plugin);
	}
	//---------------------------------------------------------------------
	void EngineManager::preRunning()
	{
		loadPlugins();
		loadResources();
	}
	//---------------------------------------------------------------------
	void EngineManager::run()
	{
		while (!getQuitLooping())
		{
			WindowsEventHandle::handleWindowsMsg();
			handleGamePlayLogical();
		}
	}
	//---------------------------------------------------------------------
	void EngineManager::postRunning()
	{
		clearResources();
		clearPlugins();
	}
	//---------------------------------------------------------------------
	void EngineManager::loadPlugins()
	{
		/// TODO:Need read from .cfg
#ifdef WHISPERWIND_DEBUG
		Util::string dllName("Whisperwind_D3D9RenderSystem_d.dll");
#else
		Util::string dllName("Whisperwind_D3D9RenderSystem.dll");
#endif

		HMODULE dllHandle = ::LoadLibraryExA(dllName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		IF_NULL_EXCEPTION(dllHandle, (dllName + " Loading Failed!").c_str());

		Util::DLL_LOAD_ENTRY dllLoadFunc = 
			reinterpret_cast<Util::DLL_LOAD_ENTRY>(::GetProcAddress(dllHandle, "dllLoadEntry"));
		IF_NULL_EXCEPTION(dllLoadFunc, (dllName + " doesn't have dllLoadEntry!").c_str());

		dllLoadFunc();
	}
	//---------------------------------------------------------------------
	void EngineManager::loadResources()
	{}
	//---------------------------------------------------------------------
	void EngineManager::clearResources()
	{}
	//---------------------------------------------------------------------
	void EngineManager::clearPlugins()
	{
		BOOST_FOREACH(Util::PluginPtr plugin, mPluginVector)
		{
			plugin->uninstall();
		}

		mPluginVector.clear();
	}
	//---------------------------------------------------------------------
	void EngineManager::handleGamePlayLogical()
	{}
}