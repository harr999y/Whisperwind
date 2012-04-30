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
#ifndef _D3D9_DEVICE_H_
#define _D3D9_DEVICE_H_

#include "Util.h"
#include "EngineForwardDeclare.h"
#include "D3D9ForwardDeclare.h"

/** for windows.h's warning level */
#pragma warning(push, 3)
#include <windows.h>
#pragma warning(pop)

namespace Engine
{
	class D3D9Device
	{
	public:
		explicit D3D9Device(const EngineConfigPtr & engineCfg);
		~D3D9Device();

	public:
		bool render();
		void createDevice(HWND window);
		bool isPaused();
		void createUnmanagedResource()
		{ /** TODO! */ }

	private:
		void init();
		bool reset();

	private:
		DISALLOW_COPY_AND_ASSIGN(D3D9Device);

	private:
		SET_GET_CONST_VALUE(bool, IsDeviceLost);

	private:
		EngineConfigPtr mEngineConfig;
		IDirect3DDevice9Ptr mD3DDevice;
		D3D9CapabilityPtr mCapability;
		bool mIsDeviceLost;
		bool mIsPaused;		
		D3DPRESENT_PARAMETERS mPresentParameters;  /// Keep for reset.
	};
}

#endif