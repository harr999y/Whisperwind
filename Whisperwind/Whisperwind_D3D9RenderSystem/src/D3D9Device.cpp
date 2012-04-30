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

#include "D3D9Device.h"
#include "MakeCOMPtr.h"
#include "D3D9Helper.h"
#include "WindowsHelper.h"
#include "EngineConfig.h"
#include "boost/make_shared.hpp"
#include "D3D9Capability.h"

namespace Engine
{
	//---------------------------------------------------------------------
	D3D9Device::D3D9Device(const EngineConfigPtr & engineCfg) :
	    mEngineConfig(engineCfg),
		mIsPaused(false),
		mIsDeviceLost(false)
	{
		init();
	}
	//---------------------------------------------------------------------
	D3D9Device::~D3D9Device()
	{
		mEngineConfig.reset();
		mCapability.reset();
		mD3DDevice.reset();
		mD3D.reset();
	}
	//---------------------------------------------------------------------
	void D3D9Device::init()
	{
		MEMORY_ZERO(&mPresentParameters, sizeof(D3DPRESENT_PARAMETERS));

		mD3D = Util::MakeCOMPtr(Direct3DCreate9(D3D_SDK_VERSION));
		IF_NULL_EXCEPTION(mD3D, "Create mD3D failed!");

		/// Here we do needed checks.
		mCapability = boost::make_shared<D3D9Capability>(mD3D);
	}
	//---------------------------------------------------------------------
	void D3D9Device::createDevice(HWND window)
	{
		Util::s_int vpType = 0;
		if (mCapability->getD3DCaps().DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
			vpType = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else
			vpType = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		mPresentParameters.AutoDepthStencilFormat = mCapability->getSupportedFomat(DEPTH_STENCIL);
		mPresentParameters.BackBufferCount = 1;
		mPresentParameters.BackBufferFormat = mCapability->getSupportedFomat(BACK_BUFFER);
		mPresentParameters.BackBufferHeight = mEngineConfig->getResolutionPair().second;
		mPresentParameters.BackBufferWidth = mEngineConfig->getResolutionPair().first;
		mPresentParameters.EnableAutoDepthStencil = true;
		mPresentParameters.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
		mPresentParameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		mPresentParameters.hDeviceWindow = window;
		mPresentParameters.MultiSampleQuality = mEngineConfig->getMultiSampleQuality();
		mPresentParameters.MultiSampleType = static_cast<D3DMULTISAMPLE_TYPE>(mEngineConfig->getMultiSampleType());
		mPresentParameters.SwapEffect = mEngineConfig->getFullScreen() ? D3DSWAPEFFECT_DISCARD : D3DSWAPEFFECT_COPY;
		mPresentParameters.Windowed = !mEngineConfig->getFullScreen();
		/// NOTE:As OGRE comment followed:
		// NB not using vsync in windowed mode in D3D9 can cause jerking at low 
		// frame rates no matter what buffering modes are used (odd - perhaps a
		// timer issue in D3D9 since GL doesn't suffer from this) 
		// low is < 200fps in this context
		mPresentParameters.PresentationInterval = (mEngineConfig->getFullScreen() && mEngineConfig->getVSync()) 
			? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE; 

		D3DDEVTYPE devType =  D3DDEVTYPE_HAL;
		UINT adapterNum = D3DADAPTER_DEFAULT;
		/// use perfHUD
		if (mEngineConfig->getPerfHUD())
		{
			for (UINT adapter = 0; adapter < mD3D->GetAdapterCount(); ++adapter)
			{
				D3DADAPTER_IDENTIFIER9 identifier;
				mD3D->GetAdapterIdentifier(adapter, 0, &identifier);
				if (Util::String(identifier.Description).find("PerfHUD") != Util::String::npos)
				{
					adapterNum = adapter;
					devType = D3DDEVTYPE_REF;
					break;
				}
			}
		}

		HRESULT hr = S_OK;
		IDirect3DDevice9 * d3dDevice = NULL;
		bool keepCircle = true; /// To make compiler happy.
		/** Do as UE3 does. 
		@note
		    This way smells very bad.Now I just do it,and waiting for more tests.
		*/
		while (keepCircle)
		{
			hr = mD3D->CreateDevice(adapterNum, devType, window, vpType, &mPresentParameters, &d3dDevice);
			DX_IF_FAILED_DEBUG_PRINT(hr);
			if (hr != D3DERR_DEVICELOST)
				break;

			::Sleep(500);
		}
		mD3DDevice = Util::MakeCOMPtr(d3dDevice);

		WHISPERWIND_LOG(TO_UNICODE("Create device done!"));
	}
	//---------------------------------------------------------------------
	bool D3D9Device::isPaused()
	{
		return getIsDeviceLost(); // ||
	}
	//---------------------------------------------------------------------
	bool D3D9Device::render()
	{
		if (getIsDeviceLost())
		{
			if (!reset())
				return true; /// Here I don't return false,because it DOESNOT have the same meaning.
		}

		DX_IF_FAILED_DEBUG_PRINT(mD3DDevice->Clear(
			0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, ColorPredefines::WHITE, 1.0f, 0));

		DX_IF_FAILED_DEBUG_PRINT(mD3DDevice->BeginScene());

		DX_IF_FAILED_DEBUG_PRINT(mD3DDevice->EndScene());
		
		HRESULT hr;
		hr = mD3DDevice->Present(NULL, NULL, NULL, NULL);
		if (D3DERR_DEVICELOST == hr)
		{
			setIsDeviceLost(true);
		}
		else if (D3DERR_DRIVERINTERNALERROR == hr)
		{
			/// Do as DXUT does.See the comment here:http://www.koders.com/cpp/fid0B76F09FD0760D71DF1C69C93BADA2141D522C85.aspx#L3799
		    setIsDeviceLost(true);
		}

		return true;
	}
	//---------------------------------------------------------------------
	bool D3D9Device::reset()
	{
		HRESULT hr = mD3DDevice->TestCooperativeLevel();
		if (FAILED(hr))
		{
			if (D3DERR_DEVICELOST == hr)
			{
				/// Comment from DXUT as followed:
				// The device has been lost but cannot be reset at this time.
				// So wait until it can be reset.
				return false;
			}
		}

		/** Do as UE3 does. 
		@note
		    This way smells very bad.Now I just do it,and waiting for more tests.
		*/
		while (FAILED(mD3DDevice->Reset(&mPresentParameters)))
		{
			 ::Sleep(1000);
		}
		
		setIsDeviceLost(false);

		return true;
	}

}