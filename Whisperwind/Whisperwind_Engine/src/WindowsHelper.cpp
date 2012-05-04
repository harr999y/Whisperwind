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

#include "WindowsHelper.h"
#include "Plugin.h"
#include "ExceptionDefine.h"
#include "EngineManager.h"
#include "EngineConfig.h"
#include "WindowsEventHandle.h"

namespace Engine
{
	static const Util::String DLL_PREFIX("Whisperwind_");
#ifdef WHISPERWIND_DEBUG
	static const Util::String DLL_EXT("_d.dll");
#else
	static const Util::String DLL_EXT(".dll");
#endif
	//---------------------------------------------------------------------
	void WindowsHelper::loadPlugin(const Util::String & plugin)
	{
		Util::String dllName = DLL_PREFIX + plugin + DLL_EXT;
		HMODULE dllHandle = ::LoadLibraryExA(dllName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		IF_NULL_EXCEPTION(dllHandle, plugin + " dll Loading Failed!");

		Util::DLL_LOAD_ENTRY dllLoadFunc = 
			reinterpret_cast<Util::DLL_LOAD_ENTRY>(::GetProcAddress(dllHandle, "dllLoadEntry"));
		IF_NULL_EXCEPTION(dllLoadFunc, plugin + " dll doesn't have dllLoadEntry!");

		dllLoadFunc();
	}
	//---------------------------------------------------------------------
	HWND WindowsHelper::createWindow()
	{
		Util::Wstring windowName = EngineManager::getSingleton().getWindowName();
		HINSTANCE hInst = ::GetModuleHandle(NULL);

		WNDCLASSEXW wc;
		MEMORY_ZERO(&wc, sizeof(WNDCLASSEXW));
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowsEventHandle::WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInst;
		wc.hIcon = NULL;
		wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
		wc.lpszMenuName = NULL;
		wc.lpszClassName = windowName.c_str();
		wc.hIconSm = NULL;

		::RegisterClassEx(&wc);

		EngineConfigPtr engineCfg = EngineManager::getSingleton().getEngineConfig();
		bool isFullScreen = engineCfg->getFullScreen();
		Util::u_int width = engineCfg->getResolutionPair().first;
		Util::u_int height = engineCfg->getResolutionPair().second;

		Util::u_int style;
		Util::u_int styleEx = 0;
		if (!isFullScreen)
		{
			style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE;
		}
		else
		{
			style = WS_POPUP | WS_CLIPCHILDREN | WS_VISIBLE;
			styleEx = WS_EX_TOPMOST;
		}

		RECT rc = {0, 0, width, height};
		::AdjustWindowRect(&rc, style, false);

		HWND window = ::CreateWindowEx(styleEx, windowName.c_str(), windowName.c_str(), style, 
			0, 0, width, height, NULL, NULL, hInst, NULL);

		::ShowWindow(window, SW_SHOWNORMAL);
		::UpdateWindow(window);

		WindowsEventHandle::setWindow(window);

		WHISPERWIND_LOG(TO_UNICODE("Create window done!"));

		return window;
	}
}