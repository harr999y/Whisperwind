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

#include <boost/make_shared.hpp>

#include "MakeCOMPtr.h"
#include "CheckedCast.h"
#include "ExceptionDefine.h"
#include "EngineConfig.h"
#include "WindowsHelper.h"
#include "EngineManager.h"
#include "Renderable.h"
#include "RenderMappingDefines.h"
#include "RenderTexture.h"
#include "D3D9FormatMapping.h"
#include "D3D9Capability.h"
#include "D3D9Helper.h"
#include "D3D9Renderable.h"
#include "D3D9RenderSystem.h"
#include "D3D9RenderTexture.h"

namespace Engine
{
	//---------------------------------------------------------------------
	D3D9RenderSystem::D3D9RenderSystem() :
	    mWindow(NULL),
		mIsDeviceLost(false)
	{}
	//---------------------------------------------------------------------
	D3D9RenderSystem::~D3D9RenderSystem()
	{
		mEngineConfig.reset();
		mCapability.reset();
		mD3DDevice.reset();
		mD3D.reset();
		::DestroyWindow(mWindow);
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::init_impl()
	{
		mEngineConfig = EngineManager::getSingleton().getEngineConfig();

		mWindow = WindowsHelper::createWindow();

		mD3D = Util::makeCOMPtr(Direct3DCreate9(D3D_SDK_VERSION));
		IF_NULL_EXCEPTION(mD3D, "Create mD3D failed!");

		/// Here we do needed checks.
		mCapability = boost::make_shared<D3D9Capability>(mD3D);

		MEMORY_ZERO(&mPresentParameters, sizeof(D3DPRESENT_PARAMETERS));

		createDevice(mWindow);
	}
	//---------------------------------------------------------------------
	bool D3D9RenderSystem::render_impl(const RenderablePtr & renderable)
	{
		if (getIsDeviceLost())
		{
			if (!reset())
				return false;
		}

		if (!checkDeviceLostBeforeDraw())
		{
			/// Draw
			D3D9RenderablePtr d3d9Renderable= Util::checkedPtrCast<D3D9Renderable>(renderable);
 
 			mD3DDevice->SetVertexDeclaration(d3d9Renderable->getVertexBound().VertexDeclaration.get());

			std::size_t size = d3d9Renderable->getVertexBound().VertexBufferVec.size();
			for (std::size_t it = 0; it < size; ++it)
			{
				mD3DDevice->SetStreamSource(
					it, d3d9Renderable->getVertexBound().VertexBufferVec[it].get(), 0, d3d9Renderable->getVertexBound().VertexStrideVec[it]);
			}

			if (d3d9Renderable->getHasIndex())
			{
				mD3DDevice->SetIndices(d3d9Renderable->getIndexBuffer().get());
			}

			ID3DXEffectPtr effect = d3d9Renderable->getEffect();
			effect->SetTechnique(d3d9Renderable->getTechnique());

			Util::u_int passNum = 0;
			DX_IF_FAILED_DEBUG_PRINT(effect->Begin(&passNum, 0));
			for (Util::u_int passIt = 0; passIt < passNum; ++passIt)
			{
				DX_IF_FAILED_DEBUG_PRINT(effect->BeginPass(passIt));
				{
					Util::u_int primCount = D3D9Helper::getPrimCount(d3d9Renderable->getPrimType(), d3d9Renderable->getVertexBound().VertexCount);

					if (d3d9Renderable->getHasIndex())
					{
						DX_IF_FAILED_DEBUG_PRINT(mD3DDevice->DrawIndexedPrimitive(d3d9Renderable->getPrimType(), 0, 0, 
						    d3d9Renderable->getVertexBound().VertexCount, 0, primCount));
					}
					else
					{
						DX_IF_FAILED_DEBUG_PRINT(mD3DDevice->DrawPrimitive(
							d3d9Renderable->getPrimType(), 0, primCount));
					}
				}
				DX_IF_FAILED_DEBUG_PRINT(effect->EndPass());
			}
			DX_IF_FAILED_DEBUG_PRINT(effect->End());
		}

		return true;
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::beginRendering_impl()
	{
		DX_IF_FAILED_DEBUG_PRINT(mD3DDevice->BeginScene());
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::endRendering_impl()
	{
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
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::clearFrame_impl(Util::u_int flag, Util::real zValue, Util::u_int stencilValue)
	{
		Util::u_int d3d9Flag = D3D9FormatMappingFactory::getD3D9ClearFrameFlag(flag);

		DX_IF_FAILED_DEBUG_PRINT(mD3DDevice->Clear(
			0, NULL, d3d9Flag, ColorPredefines::WHITE, zValue, stencilValue));
	}
	//---------------------------------------------------------------------
	bool D3D9RenderSystem::isPaused_impl()
	{
		return getIsDeviceLost(); // ||
	}
	//---------------------------------------------------------------------
	RenderablePtr D3D9RenderSystem::createRenderable_impl(const RenderableMappingPtr & rm)
	{
		RenderablePtr d3d9Renderable = D3D9Helper::createD3D9Renderable(mD3DDevice, mEffectMap, rm);

		return d3d9Renderable;
	}
	//---------------------------------------------------------------------
	RenderTexturePtr D3D9RenderSystem::createTexture_impl(const TextureMappingPtr & tm)
	{
		Util::u_int usage = D3D9FormatMappingFactory::getD3D9TextureCreateFlag(tm->Usage);
		D3DFORMAT fmt = D3D9FormatMappingFactory::getD3D9PixelFormat(tm->Format);
		IDirect3DTexture9 * texture = NULL;

		IF_FAILED_EXCEPTION(mD3DDevice->CreateTexture(tm->Width, tm->Height, tm->Levels, usage, fmt, D3DPOOL_DEFAULT, &texture, NULL), 
			"Create texture failed!");

		IDirect3DTexture9Ptr texPtr = Util::makeCOMPtr(texture);

		D3D9RenderTexturePtr d3d9RtPtr = boost::make_shared<D3D9RenderTexture>();
		d3d9RtPtr->setTexture(texPtr);

		return d3d9RtPtr;
	}
	//---------------------------------------------------------------------
	RenderTexturePtr D3D9RenderSystem::createTextureFromFile_impl(const Util::Wstring & path)
	{
		IDirect3DTexture9 * texture = NULL;

		Util::String str;
		Util::WstringToString(path, str);
		IF_FAILED_EXCEPTION(D3DXCreateTextureFromFile(mD3DDevice.get(), path.c_str(), &texture), str + " create failed!");

		IDirect3DTexture9Ptr texturePtr = Util::makeCOMPtr(texture);

		D3D9RenderTexturePtr d3d9RtPtr = boost::make_shared<D3D9RenderTexture>();
		d3d9RtPtr->setTexture(texturePtr);

		return d3d9RtPtr;
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::createDevice(HWND window)
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
		mD3DDevice = Util::makeCOMPtr(d3dDevice);

		D3DVIEWPORT9 viewport = 
		{ 0, 0, mEngineConfig->getResolutionPair().first, mEngineConfig->getResolutionPair().second, 0.0f, 1.0f };

		mD3DDevice->SetViewport(&viewport);

		WHISPERWIND_LOG(TO_UNICODE("Create device done!"));
	}
	//---------------------------------------------------------------------
	bool D3D9RenderSystem::reset()
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
	//---------------------------------------------------------------------
	bool D3D9RenderSystem::checkDeviceLostBeforeDraw()
	{
		return (mD3DDevice->TestCooperativeLevel() != S_OK);
	}
}