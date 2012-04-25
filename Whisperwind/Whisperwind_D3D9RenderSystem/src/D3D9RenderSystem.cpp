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

#include "D3D9RenderSystem.h"
#include "WindowsEventHandle.h"
#include "EngineManager.h"
#include "EngineConfig.h"

namespace Engine
{
	//---------------------------------------------------------------------
	void D3D9RenderSystem::init()
	{
		createWindow();
		createDevice();
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::createWindow()
	{
		/// TODO:The parameter need to read from .cfg!
		Util::Wstring windowName = EngineManager::getSingleton().getWindowName();
		HINSTANCE hInst = ::GetModuleHandle(NULL);

		WNDCLASSEXW wc;
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowsEventHandle::WndProc;
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

		bool isFullScreen = mEngineConfig->getFullScreen();
		Util::u_int width = mEngineConfig->getResolutionPair().first;
		Util::u_int height = mEngineConfig->getResolutionPair().second;

		Util::u_int style;
		if (!isFullScreen)
			 style = WS_OVERLAPPEDWINDOW;
		else
			style = WS_POPUP;

		RECT rc = {0, 0, width, height};
		::AdjustWindowRect(&rc, style, false);	

		HWND window = ::CreateWindow(windowName.c_str(), windowName.c_str(), style, 
			0, 0, width, height, NULL, NULL, hInst, NULL);

		::ShowWindow(window, SW_SHOWNORMAL);
		::UpdateWindow(window);

		setWindowHWND(window);

		WHISPERWIND_LOG(TO_UNICODE("Create window done!"));
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::createDevice()
	{

	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::setWindowHWND(HWND window)
	{
		mWindow = window;
		WindowsEventHandle::setWindow(window);
	}
	//---------------------------------------------------------------------
	bool D3D9RenderSystem::render()
	{


		return true;
	}
}