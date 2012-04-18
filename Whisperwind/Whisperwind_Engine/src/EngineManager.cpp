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

namespace Engine
{
	//---------------------------------------------------------------------
	void EngineManager::preRunning(const Util::wstring & appName)
	{
		createWindow(appName);
		createDevice();
		loadPlugins();
		loadResources();
	}
	//---------------------------------------------------------------------
	void EngineManager::run()
	{
		while (!getQuitLooping())
		{
			handleWindowsMsg();
			handleGamePlayLogical();
		}
	}
	//---------------------------------------------------------------------
	void EngineManager::postRunning()
	{
		clearResources();
		clearPlugins();
		destroyDevice();
		destroyWindow();
	}
	//---------------------------------------------------------------------
	void EngineManager::createWindow(const Util::wstring & windowName)
	{
		/// TODO:Write here first,need move to someplace.
		HINSTANCE hInst = ::GetModuleHandle(NULL);

		WNDCLASSEXW wc;
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = sizeof(this);
		wc.hInstance = hInst;
		wc.hIcon = NULL;
		wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
		wc.lpszMenuName = NULL;
		wc.lpszClassName = windowName.c_str();
		wc.hIconSm = NULL;

		::RegisterClassEx(&wc);

		Util::u_int style = WS_OVERLAPPEDWINDOW;
		RECT rc = {0, 0, 1024, 768};
		::AdjustWindowRect(&rc, style, false);

		mWindow = ::CreateWindow(windowName.c_str(), windowName.c_str(), style, 
			                                                  0, 0, 1024, 768, NULL, NULL, hInst, NULL);
		::ShowWindow(mWindow, SW_SHOWNORMAL);
		::UpdateWindow(mWindow);
	}
	//---------------------------------------------------------------------
	void EngineManager::createDevice()
	{}
	//---------------------------------------------------------------------
	void EngineManager::loadPlugins()
	{}
	//---------------------------------------------------------------------
	void EngineManager::loadResources()
	{}
	//---------------------------------------------------------------------
	void EngineManager::clearResources()
	{}
	//---------------------------------------------------------------------
	void EngineManager::clearPlugins()
	{}
	//---------------------------------------------------------------------
	void EngineManager::destroyDevice()
	{}
	//---------------------------------------------------------------------
	void EngineManager::destroyWindow()
	{}
	//---------------------------------------------------------------------
	void EngineManager::handleGamePlayLogical()
	{}
	//---------------------------------------------------------------------
	void EngineManager::handleWindowsMsg()
	{
		/// TODO:Write here first,need move to someplace.
		static MSG msg;
		memset(&msg, 0, sizeof(MSG));
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	//---------------------------------------------------------------------
	LRESULT EngineManager::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		/// TODO:First we do nothing,it'll change after days.
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}