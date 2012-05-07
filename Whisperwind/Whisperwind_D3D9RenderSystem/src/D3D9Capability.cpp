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
#include "D3D9Capability.h"

namespace Engine
{
	//---------------------------------------------------------------------
	D3D9Capability::D3D9Capability(const IDirect3D9Ptr & d3d) :
        mD3D(d3d)
	{
		init();
	}
	//---------------------------------------------------------------------
	void D3D9Capability::init()
	{
		mD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &mD3DCaps);

		mCapVec.resize(CAPABILITIES_MAX);
		/// Fill default value.
		mCapVec[UNKNOWN_FORMAT].SupportedFormat = D3DFMT_UNKNOWN;
		mCapVec[DEPTH_STENCIL].SupportedFormat = D3DFMT_D24S8;
		mCapVec[BACK_BUFFER].SupportedFormat = D3DFMT_X8R8G8B8; /// TODO!

		doChecks();
	}
	//---------------------------------------------------------------------
	void D3D9Capability::doChecks()
	{
		/// Here we do all the checks.
		if (FAILED(mD3D->CheckDeviceFormat(mD3DCaps.AdapterOrdinal, mD3DCaps.DeviceType, mCapVec[BACK_BUFFER].SupportedFormat,
		    D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8)))
		{
			mCapVec[DEPTH_STENCIL].SupportedFormat = D3DFMT_D24X8;
		}
	}
	//---------------------------------------------------------------------
	D3DFORMAT D3D9Capability::getSupportedFomat(Capabilities cap) const
	{
		WHISPERWIND_ASSERT((cap < CAPABILITIES_MAX) && (cap >= UNKNOWN_FORMAT));

		return mCapVec[cap].SupportedFormat;
	}

}