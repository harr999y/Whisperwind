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

#include "D3D9RenderSystem.h"
#include "WindowsEventHandle.h"
#include "WindowsHelper.h"
#include "EngineManager.h"
#include "EngineConfig.h"
#include "MakeCOMPtr.h"
#include "D3D9Helper.h"

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
		Util::Wstring windowName = EngineManager::getSingleton().getWindowName();
		HINSTANCE hInst = ::GetModuleHandle(NULL);

		WNDCLASSEXW wc;
		MEMORY_ZERO(&wc, sizeof(WNDCLASSEXW));
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

		setWindowHWND(window);

		WHISPERWIND_LOG(TO_UNICODE("Create window done!"));
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::createDevice()
	{
		IDirect3D9Ptr d3d = Util::MakeCOMPtr(Direct3DCreate9(D3D_SDK_VERSION));
		IF_NULL_EXCEPTION(d3d, "Create d3d failed!");

		D3DCAPS9 d3dCaps;
		d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps);

		Util::s_int vpType = 0;
		if (d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
			vpType = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else
			vpType = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		D3DPRESENT_PARAMETERS presentPara;
		MEMORY_ZERO(&presentPara, sizeof(D3DPRESENT_PARAMETERS));
		presentPara.AutoDepthStencilFormat = D3DFMT_D24S8;
		presentPara.BackBufferCount = 1;
		presentPara.BackBufferFormat = D3DFMT_X8R8G8B8;
		presentPara.BackBufferHeight = mEngineConfig->getResolutionPair().second;
		presentPara.BackBufferWidth = mEngineConfig->getResolutionPair().first;
		presentPara.EnableAutoDepthStencil = true;
		presentPara.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
		presentPara.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		presentPara.hDeviceWindow = mWindow;
		presentPara.MultiSampleQuality = mEngineConfig->getMultiSampleQuality();
		presentPara.MultiSampleType = static_cast<D3DMULTISAMPLE_TYPE>(mEngineConfig->getMultiSampleType());
		presentPara.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		presentPara.SwapEffect = D3DSWAPEFFECT_DISCARD;
		presentPara.Windowed = !mEngineConfig->getFullScreen();

		IDirect3DDevice9 * d3dDevice;
		IF_FAILED_EXCEPTION(d3d->CreateDevice(D3DADAPTER_DEFAULT, 
			D3DDEVTYPE_HAL, mWindow, vpType, &presentPara, &d3dDevice),
			"Create device failed!");
		mD3DDevice = Util::MakeCOMPtr(d3dDevice);

		WHISPERWIND_LOG(TO_UNICODE("Create device done!"));
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
		DX_IF_FAILED_DEBUG_PRINT(mD3DDevice->Clear(
			0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, ColorPredefines::WHITE, 1.0f, 0));

		DX_IF_FAILED_DEBUG_PRINT(mD3DDevice->BeginScene());

		DX_IF_FAILED_DEBUG_PRINT(mD3DDevice->EndScene());
		DX_IF_FAILED_DEBUG_PRINT(mD3DDevice->Present(NULL, NULL, NULL, NULL));

		return true;
	}
}