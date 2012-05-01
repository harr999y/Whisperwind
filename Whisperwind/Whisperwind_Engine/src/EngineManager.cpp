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

#include "EngineManager.h"
#include "Plugin.h"
#include "RenderSystem.h"
#include "WindowsEventHandle.h"
#include "WindowsHelper.h"
#include "boost/foreach.hpp"
#include "XmlReader.h"
#include "EngineConfig.h"
#include "PluginConfig.h"
#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"
#include "StringConverter.h"
#include "Profile.h"
#include "Timer.h"

namespace Engine
{
	//---------------------------------------------------------------------
#ifdef WHISPERWIND_DEBUG
	static const Util::String PLUGIN_CONFIG_PATH("../config/Plugin_d.cfg");
	static const Util::String ENGINE_CONFIG_PATH("../config/EngineConfig_d.cfg");
#else
	static const Util::String PLUGIN_CONFIG_PATH("../config/Plugin.cfg");
	static const Util::String ENGINE_CONFIG_PATH("../config/EngineConfig.cfg");
#endif
	//---------------------------------------------------------------------
	EngineManager EngineManager::mSingleton;
	EngineManager & EngineManager::getSingleton()
	{
		return mSingleton;
	}
	//---------------------------------------------------------------------
	EngineManager::EngineManager()
	{
		init();
	}
	//---------------------------------------------------------------------
	EngineManager::~EngineManager()
	{
		/// NOTE:Here we'd better DONOT destroy or release anything,because this class is global--Only been destructed when some
		/// threads destroyed and some dlls detached!
	}
	//---------------------------------------------------------------------
	void EngineManager::init()
	{
		setQuitLooping(false);
		setEngineConfig(boost::make_shared<EngineConfig>(ENGINE_CONFIG_PATH));
		setPluginConfig(boost::make_shared<PluginConfig>(PLUGIN_CONFIG_PATH));
		mTimer = boost::make_shared<Util::WindowsTimer>();

		loadConfigs();
	}
	//---------------------------------------------------------------------
	void EngineManager::loadConfigs()
	{
		mEngineConfig->parse();
		WHISPERWIND_LOG(TO_UNICODE("Engine config parse done!"));

		mPluginConfig->parse();
		WHISPERWIND_LOG(TO_UNICODE("Plugin config parse done!"));
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
			handleLogical();

			IF_FALSE_EXCEPTION(mRenderSystem->render(mTimer->getElapsedTime()), "Render failed!");
		}
	}
	//---------------------------------------------------------------------
	void EngineManager::postRunning()
	{
		clearResources();
		clearPlugins();
		clearConfigs();

		WHISPERWIND_LOG(TO_UNICODE("Quit engine done!"));
	}
	//---------------------------------------------------------------------
	void EngineManager::installPlugin(const Util::PluginPtr & plugin)
	{
		plugin->install();

		mPluginVector.push_back(plugin);
	}
	//---------------------------------------------------------------------
	void EngineManager::loadPlugins()
	{
		Util::StringVector strVec = mPluginConfig->getStringVector();
		Util::Wstring wstr;
		BOOST_FOREACH(Util::String & str, strVec)
		{
			WindowsHelper::loadPlugin(str);

			wstr.clear();
			Util::StringToWstring(str, wstr);
			WHISPERWIND_LOG(wstr + TO_UNICODE(" plugin load done!"));
		}

		mPluginConfig.reset();
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
		BOOST_FOREACH(Util::PluginPtr & plugin, mPluginVector)
		{
			plugin->uninstall();

			WHISPERWIND_LOG(plugin->getName() + TO_UNICODE(" plugin unload done!"));
		}

		mPluginVector.clear();
	}
	//---------------------------------------------------------------------
	void EngineManager::clearConfigs()
	{
		mEngineConfig.reset();
		mPluginConfig.reset();
	}
	//---------------------------------------------------------------------
	void EngineManager::handleLogical()
	{}
}