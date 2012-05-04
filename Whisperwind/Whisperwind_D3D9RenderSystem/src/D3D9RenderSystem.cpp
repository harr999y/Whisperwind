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
#include "EngineConfig.h"
#include "boost/make_shared.hpp"
#include "WindowsHelper.h"
#include "EngineManager.h"
#include "Renderable.h"
#include "MakeCOMPtr.h"
#include "D3D9FormatMapping.h"
#include "ExceptionDefine.h"
#include "D3D9Capability.h"
#include "D3D9Helper.h"
#include "CheckedCast.h"
#include "D3D9Renderable.h"
#include "RenderMappingDefines.h"
#include <algorithm>

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
	void D3D9RenderSystem::init()
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
	bool D3D9RenderSystem::render(const RenderablePtr & renderable)
	{
		/// Important way to save CPU when minimized or something else.
 		if (isPaused())
 			::Sleep(1);

		if (getIsDeviceLost())
		{
			if (!reset())
				return true; /// Here I don't return false,because it DOESNOT have the same meaning.
		}

		DX_IF_FAILED_DEBUG_PRINT(mD3DDevice->Clear(
			0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, ColorPredefines::WHITE, 1.0f, 0));

		DX_IF_FAILED_DEBUG_PRINT(mD3DDevice->BeginScene());

		if (!checkDeviceLostBeforeDraw())
		{
			/// Draw
			D3D9RenderablePtr d3d9Renderable= Util::checkedPtrCast<D3D9Renderable>(renderable);
 
 			mD3DDevice->SetVertexDeclaration(d3d9Renderable->getVertexBound().VertexDeclaration.get());
			mD3DDevice->SetStreamSource(0, d3d9Renderable->getVertexBound().VertexBuffer.get(), 0, d3d9Renderable->getVertexBound().VertexStride);

			if (d3d9Renderable->getHasIndex())
			{
				mD3DDevice->SetIndices(d3d9Renderable->getIndexBuffer().get());
			}

			ID3DXEffectPtr effect = d3d9Renderable->getEffect();
			effect->SetTechnique(d3d9Renderable->getTechnique());

			Util::u_int passNum = 0;
			effect->Begin(&passNum, 0);
			for (Util::u_int passIt = 0; passIt < passNum; ++passIt)
			{
				effect->BeginPass(passIt);
				{
					if (d3d9Renderable->getHasIndex())
						mD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, d3d9Renderable->getVertexBound().VertexBufSize / d3d9Renderable->getVertexBound().VertexStride, 0, 2);
					else
						mD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
				}
				effect->EndPass();
			}
			effect->End();
		}

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
	RenderablePtr D3D9RenderSystem::createRenderable(const RenderableMappingPtr & rm)
	{
		D3D9RenderablePtr renderable = boost::make_shared<D3D9Renderable>();

		/// Vertex Declaration
		{
			renderable->getVertexBound().VertexDeclaration = D3D9FormatMappingFactory::createD3D9VertexDeclaration(mD3DDevice, rm->VertexElemVec);
		}

		/// VB
		{
			IDirect3DVertexBuffer9 * vb = NULL;
			DX_IF_FAILED_DEBUG_PRINT(mD3DDevice->CreateVertexBuffer(rm->VertexData.DataSize, 0, 0, D3DPOOL_MANAGED, &vb, NULL));
			{
				void * buf;
				DX_IF_FAILED_DEBUG_PRINT(vb->Lock(0, 0, &buf, 0));
				std::copy(static_cast<const Util::u_int8 *>(rm->VertexData.Data), 
					static_cast<const Util::u_int8 *>(rm->VertexData.Data) + rm->VertexData.DataSize, static_cast<Util::u_int8 *>(buf));
				vb->Unlock();
			}
			IDirect3DVertexBuffer9Ptr vbPtr = Util::makeCOMPtr(vb);
			renderable->getVertexBound().VertexBuffer = vbPtr;
			renderable->getVertexBound().VertexBufSize = rm->VertexData.DataSize;
			renderable->getVertexBound().VertexStride = rm->VertexData.Stride;
		}

		/// IB
		if (rm->HasIndex)
		{
			IDirect3DIndexBuffer9 * ib = NULL;
			DX_IF_FAILED_DEBUG_PRINT(mD3DDevice->CreateIndexBuffer(rm->IndexData.DataSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &ib, NULL));
			{
				void * buf;
				DX_IF_FAILED_DEBUG_PRINT(ib->Lock(0, 0, &buf, 0));
				std::copy(static_cast<const Util::u_int8 *>(rm->IndexData.Data), 
					static_cast<const Util::u_int8 *>(rm->IndexData.Data) + rm->IndexData.DataSize, static_cast<Util::u_int8 *>(buf));
				ib->Unlock();
			}
		}

		/// Effect
		{
			if (mEffectMap.find(rm->EffectName) == mEffectMap.end())
			{
#ifdef WHISPERWIND_DEBUG
				DWORD shaderFlags = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;
#else
				DWORD shaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL2;
#endif
				ID3DXEffect * effect = NULL;
				DX_IF_FAILED_DEBUG_PRINT(D3DXCreateEffectFromFile(mD3DDevice.get(), (Util::Wstring(TO_UNICODE("../media/Effects/")) + rm->EffectName).c_str(), 
					NULL, NULL, shaderFlags, NULL, &effect, NULL));
				ID3DXEffectPtr effectPtr = Util::makeCOMPtr(effect);
				mEffectMap[rm->EffectName] = effectPtr;
				renderable->setEffect(effectPtr);
			}
			else
			{
				renderable->setEffect(mEffectMap[rm->EffectName]);
			}
		}

		/// Technique
		{
			renderable->setTechnique(renderable->getEffect()->GetTechniqueByName(rm->TechniqueName.c_str()));
		}

		return renderable;
	}
	//---------------------------------------------------------------------
	bool D3D9RenderSystem::isPaused()
	{
		return getIsDeviceLost(); // ||
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