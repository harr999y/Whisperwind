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

#include "DebugDefine.h"
#include "EngineManager.h"
#include "WindowsEventHandle.h"

namespace Engine
{
	HWND WindowsEventHandle::mWindow;
	//---------------------------------------------------------------------
	void WindowsEventHandle::handleWindowsMsg()
	{
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	//---------------------------------------------------------------------
	LRESULT CALLBACK WindowsEventHandle::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch(uMsg)
		{
		case WM_ACTIVATE:
			{
				break;
			}
		case WM_CLOSE:
			{
				EngineManager::getSingleton().setQuitLooping(true);
				break;
			}
		case WM_CHAR:
			{
				switch(wParam)
				{
				case VK_ESCAPE:
					{
						EngineManager::getSingleton().setQuitLooping(true);
						break;
					}
				}
				break;
			}
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	//---------------------------------------------------------------------
	const HWND WindowsEventHandle::getWindow()
	{
		return mWindow;
	}
	//---------------------------------------------------------------------
	const void WindowsEventHandle::getWindow(void * data)
	{
		WHISPERWIND_ASSERT(data);
		HWND * hwnd = static_cast<HWND *>(data);
		*hwnd = mWindow;
	}
	//---------------------------------------------------------------------
	void WindowsEventHandle::setWindow(HWND window)
	{
		mWindow = window;
	}
}