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
#include "XmlReader.h"

namespace Engine
{
	//---------------------------------------------------------------------
#ifdef WHISPERWIND_DEBUG
	static const Util::String PLUGIN_CONFIG_PATH("../config/Plugin_d.cfg");
#else
	static const Util::String PLUGIN_CONFIG_PATH("../config/Plugin.cfg");
#endif
	static const Util::String NODE_NAME("Plugin");
	static const Util::String ATTRIBUTE_NAME("name");
	static const Util::String DLL_PREFIX("Whisperwind_");
	static const Util::String DLL_EXT(".dll");
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
		Util::XmlReaderPtr xmlReaderPtr = boost::make_shared<Util::XmlReader>(PLUGIN_CONFIG_PATH);
		IF_NULL_EXCEPTION(xmlReaderPtr, "XmlReader create failed!");

		IF_FALSE_EXCEPTION(xmlReaderPtr->advanceFirstChildNode(NODE_NAME),
			                                  "Xml parse failed!")

		Util::StringVector strVec;
		do
		{
			strVec.push_back(xmlReaderPtr->getAttribute(ATTRIBUTE_NAME));
		}while (xmlReaderPtr->advanceNextSiblingNode(NODE_NAME));

		BOOST_FOREACH(Util::String & str, strVec)
		{
			loadPlugin(str);
		}
	}
	//---------------------------------------------------------------------
	void EngineManager::loadPlugin(const Util::String & plugin)
	{
		Util::String dllName = DLL_PREFIX + plugin + DLL_EXT;
		HMODULE dllHandle = ::LoadLibraryExA(dllName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		IF_NULL_EXCEPTION(dllHandle, "Dll Loading Failed!");

		Util::DLL_LOAD_ENTRY dllLoadFunc = 
			reinterpret_cast<Util::DLL_LOAD_ENTRY>(::GetProcAddress(dllHandle, "dllLoadEntry"));
		IF_NULL_EXCEPTION(dllLoadFunc, "Dll doesn't have dllLoadEntry!");

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
		BOOST_FOREACH(Util::PluginPtr & plugin, mPluginVector)
		{
			plugin->uninstall();
		}

		mPluginVector.clear();
	}
	//---------------------------------------------------------------------
	void EngineManager::handleGamePlayLogical()
	{}
}