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
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>

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
#include "D3D9RenderTexture.h"
#include "D3D9RenderTarget.h"
#include "D3D9RenderSystem.h"

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
		mEffectMap.clear();
		mRenderableMappingMap.clear();
		mRenderTextureFileMap.clear();
		mRenderTextureMappingMap.clear();
		mRenderTargetMappingMap.clear();
		mD3DDevice.reset();
		mD3D.reset();
		::DestroyWindow(mWindow);
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::init_impl()
	{
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

			/// set texture/param etc.
			{
				const ParamValuePairVector & pairPVVec = d3d9Renderable->getParamValuePairVec();
				BOOST_FOREACH(const ParamValuePair & pair, pairPVVec)
				{
					d3d9Renderable->setEffectParamValue(pair.first, pair.second.get());
				}

				const ParamTexturePairVector & pairPTVec = d3d9Renderable->getParamTexturePairVec();
				BOOST_FOREACH(const ParamTexturePair & pair, pairPTVec)
				{
					d3d9Renderable->setTexture(pair.first, pair.second);
				}
			}

			Util::u_int passNum = 0;
			DX_IF_FAILED_RETURN_FALSE(effect->Begin(&passNum, 0));
			for (Util::u_int passIt = 0; passIt < passNum; ++passIt)
			{
				DX_IF_FAILED_DEBUG_PRINT(effect->BeginPass(passIt));
				{
					/// TODO:Some error.
					//Util::u_int primCount = D3D9Helper::getPrimCount(d3d9Renderable->getPrimType(), d3d9Renderable->getVertexBound().VertexCount);
					Util::u_int primCount = d3d9Renderable->getPrimCount();

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
			DX_IF_FAILED_RETURN_FALSE(effect->End());
		}

		return true;
	}
	//---------------------------------------------------------------------
	bool D3D9RenderSystem::beginRendering_impl()
	{
		if (getIsDeviceLost())
		{
			if (!reset())
				return false;
		}

		DX_IF_FAILED_RETURN_FALSE(mD3DDevice->BeginScene());

		return true;
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::endRendering_impl()
	{
		DX_IF_FAILED_DEBUG_PRINT(mD3DDevice->EndScene());
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::present_impl()
	{
		HRESULT hr;
		hr = mD3DDevice->Present(NULL, NULL, NULL, NULL);
		if (D3DERR_DEVICELOST == hr)
		{
			setIsDeviceLost(true);
			onDeviceLost();
		}
		else if (D3DERR_DRIVERINTERNALERROR == hr)
		{
			/// Do as DXUT does.See the comment here:http://www.koders.com/cpp/fid0B76F09FD0760D71DF1C69C93BADA2141D522C85.aspx#L3799
			setIsDeviceLost(true);
			onDeviceLost();
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

  		if ((D3DPOOL_DEFAULT == D3D9Helper::getCreationPool(rm->VertexBound.VertexUsage)) ||
  			(D3DPOOL_DEFAULT == D3D9Helper::getCreationPool(rm->IndexBound.IndexUsage)))
		{
			if (mRenderableMappingMap.find(rm) == mRenderableMappingMap.end())
				mRenderableMappingMap.insert(RenderableMappingMap::value_type(rm, d3d9Renderable));
		}

		return d3d9Renderable;
	}
	//---------------------------------------------------------------------
	RenderTexturePtr D3D9RenderSystem::createRenderTexture_impl(const RenderTextureMappingPtr & rtm)
	{
		Util::u_int usage = D3D9FormatMappingFactory::getD3D9TextureCreateFlag(rtm->Usage);
		D3DFORMAT fmt = D3D9FormatMappingFactory::getD3D9PixelFormat(rtm->Format);
		IDirect3DTexture9 * texture = NULL;

		IF_FAILED_EXCEPTION(mD3DDevice->CreateTexture(rtm->Width, rtm->Height, rtm->Levels, usage, fmt, D3DPOOL_DEFAULT, &texture, NULL), 
			"Create texture failed!");

		IDirect3DTexture9Ptr texPtr = Util::makeCOMPtr(texture);

		D3D9RenderTexturePtr d3d9RtPtr = boost::make_shared<D3D9RenderTexture>();
		d3d9RtPtr->setTexture(texPtr);

		if (mRenderTextureMappingMap.find(rtm) == mRenderTextureMappingMap.end())
			mRenderTextureMappingMap.insert(RenderTextureMappingMap::value_type(rtm, d3d9RtPtr));

		return d3d9RtPtr;
	}
	//---------------------------------------------------------------------
	RenderTexturePtr D3D9RenderSystem::createRenderTextureFromFile_impl(const Util::Wstring & path)
	{
		/// For repetitive creation.
		if (!getIsDeviceLost() && mRenderTextureFileMap.find(path) != mRenderTextureFileMap.end())
		{
			RenderTextureWeakPtr textureWeak = mRenderTextureFileMap[path];
			if (!textureWeak.expired())
			{
				RenderTexturePtr texture = textureWeak.lock();
				return texture;
			}
		}

		IDirect3DTexture9 * texture = NULL;

		IF_FAILED_EXCEPTION(D3DXCreateTextureFromFile(mD3DDevice.get(), path.c_str(), &texture), Util::WstringToString(path) + " create failed!");

		IDirect3DTexture9Ptr texturePtr = Util::makeCOMPtr(texture);

		D3D9RenderTexturePtr d3d9RtPtr = boost::make_shared<D3D9RenderTexture>();
		d3d9RtPtr->setTexture(texturePtr);

		if (mRenderTextureFileMap.find(path) == mRenderTextureFileMap.end())
			mRenderTextureFileMap.insert(RenderTextureFileMap::value_type(path, d3d9RtPtr));

		return d3d9RtPtr;
	}
	//---------------------------------------------------------------------
	RenderTargetPtr D3D9RenderSystem::createRenderTarget_impl(const RenderTargetMappingPtr & rtm)
	{
		D3DFORMAT fmt = D3D9FormatMappingFactory::getD3D9PixelFormat(rtm->Format);
		IDirect3DSurface9 * surface = NULL;

		IF_FAILED_EXCEPTION(mD3DDevice->CreateRenderTarget(rtm->Width, rtm->Height, fmt, 
			static_cast<D3DMULTISAMPLE_TYPE>(rtm->MultiSampleType), rtm->MultiSampleQuality, false, &surface, NULL), "Render target create failed!");

		IDirect3DSurface9Ptr surfacePtr = Util::makeCOMPtr(surface);

		D3D9RenderTargetPtr d3d9RtPtr = boost::make_shared<D3D9RenderTarget>();
		d3d9RtPtr->setSurface(surfacePtr);

		if (mRenderTargetMappingMap.find(rtm) == mRenderTargetMappingMap.end())
			mRenderTargetMappingMap.insert(RenderTargetMappingMap::value_type(rtm, d3d9RtPtr));

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

			EngineManager::getSingleton().sleep(500);
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
			EngineManager::getSingleton().sleep(1000);
		}
		
		onDeviceReset();

		setIsDeviceLost(false);

		return true;
	}
	//---------------------------------------------------------------------
	bool D3D9RenderSystem::checkDeviceLostBeforeDraw()
	{
		return (mD3DDevice->TestCooperativeLevel() != S_OK);
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::onDeviceLost()
	{
		/// Effect
		{
			BOOST_AUTO(it, mEffectMap.begin());
			for (it; it != mEffectMap.end(); ++it)
			{
				it->second->OnLostDevice();
			}
		}

		/// Renderable
		{
			BOOST_AUTO(it, mRenderableMappingMap.begin());
			for (it; it != mRenderableMappingMap.end(); ++it)
			{
				RenderableWeakPtr renderableWeak = it->second;
				if (!renderableWeak.expired())
				{
					RenderablePtr renderable = renderableWeak.lock();
					D3D9RenderablePtr d3d9Renderable = Util::checkedPtrCast<D3D9Renderable>(renderable);
					d3d9Renderable->onDeviceLost();
				}
				else
				{
					mRenderableMappingMap.erase(it);
				}
			}
		}

		/// Texture from file
		{
			BOOST_AUTO(it, mRenderTextureFileMap.begin());
			for (it; it != mRenderTextureFileMap.end(); ++it)
			{
				RenderTextureWeakPtr textureWeak = it->second;
				if (!textureWeak.expired())
				{
					RenderTexturePtr texture = textureWeak.lock();
					D3D9RenderTexturePtr d3d9Texture = Util::checkedPtrCast<D3D9RenderTexture>(texture);
					d3d9Texture->onDeviceLost();
				}
				else
				{
					mRenderTextureFileMap.erase(it);
				}
			}
		}

		/// Texture from creating
		{
			BOOST_AUTO(it, mRenderTextureMappingMap.begin());
			for (it; it != mRenderTextureMappingMap.end(); ++it)
			{
				RenderTextureWeakPtr textureWeak = it->second;
				if (!textureWeak.expired())
				{
					RenderTexturePtr texture = textureWeak.lock();
					D3D9RenderTexturePtr d3d9Texture = Util::checkedPtrCast<D3D9RenderTexture>(texture);
					d3d9Texture->onDeviceLost();
				}
				else
				{
					mRenderTextureMappingMap.erase(it);
				}
			}
		}

		/// Render target
		{
			BOOST_AUTO(it, mRenderTargetMappingMap.begin());
			for (it; it != mRenderTargetMappingMap.end(); ++it)
			{
				RenderTargetWeakPtr targetWeak = it->second;
				if (!targetWeak.expired())
				{
					RenderTargetPtr target = targetWeak.lock();
					D3D9RenderTargetPtr d3d9Target = Util::checkedPtrCast<D3D9RenderTarget>(target);
					d3d9Target->onDeviceLost();
				}
				else
				{
					mRenderTargetMappingMap.erase(it);
				}
			}
		}
	}
	//---------------------------------------------------------------------
	void D3D9RenderSystem::onDeviceReset()
	{
		/// Effect
		{
			BOOST_AUTO(it, mEffectMap.begin());
			for (it; it != mEffectMap.end(); ++it)
			{
				it->second->OnResetDevice();
			}
		}

		/// Renderable
		{
			BOOST_AUTO(it, mRenderableMappingMap.begin());
			for (it; it != mRenderableMappingMap.end(); ++it)
			{
				const RenderableMappingPtr & renderableMapping = it->first;
				RenderableWeakPtr renderableWeak = it->second;
				if (!renderableWeak.expired())
				{
					RenderablePtr renderable = renderableWeak.lock();
					D3D9RenderablePtr d3d9Renderable = Util::checkedPtrCast<D3D9Renderable>(renderable);

					D3D9RenderablePtr newD3d9Renderable = Util::checkedPtrCast<D3D9Renderable>(this->createRenderable(renderableMapping));
					d3d9Renderable->onDeviceReset(newD3d9Renderable);
				}
				else
				{
					mRenderableMappingMap.erase(it);
				}
			}
		}

		/// Texture from file
		{
			BOOST_AUTO(it, mRenderTextureFileMap.begin());
			for (it; it != mRenderTextureFileMap.end(); ++it)
			{
				const Util::Wstring & wstr = it->first;
				RenderTextureWeakPtr textureWeak = it->second;
				if (!textureWeak.expired())
				{
					RenderTexturePtr texture = textureWeak.lock();
					D3D9RenderTexturePtr d3d9Texture = Util::checkedPtrCast<D3D9RenderTexture>(texture);

					D3D9RenderTexturePtr newD3d9Texture = Util::checkedPtrCast<D3D9RenderTexture>(this->createRenderTextureFromFile(wstr));
					d3d9Texture->onDeviceReset(newD3d9Texture);
				}
				else
				{
					mRenderTextureFileMap.erase(it);
				}
			}
		}

		/// Texture from creating
		{
			BOOST_AUTO(it, mRenderTextureMappingMap.begin());
			for (it; it != mRenderTextureMappingMap.end(); ++it)
			{
				const RenderTextureMappingPtr & textureMapping = it->first;
				RenderTextureWeakPtr textureWeak = it->second;
				if (!textureWeak.expired())
				{
					RenderTexturePtr texture = textureWeak.lock();
					D3D9RenderTexturePtr d3d9Texture = Util::checkedPtrCast<D3D9RenderTexture>(texture);

					D3D9RenderTexturePtr newD3d9Texture = Util::checkedPtrCast<D3D9RenderTexture>(this->createRenderTexture(textureMapping));
					d3d9Texture->onDeviceReset(newD3d9Texture);
				}
				else
				{
					mRenderTextureMappingMap.erase(it);
				}
			}
		}

		/// Render target
		{
			BOOST_AUTO(it, mRenderTargetMappingMap.begin());
			for (it; it != mRenderTargetMappingMap.end(); ++it)
			{
				const RenderTargetMappingPtr & targetMapping = it->first;
				RenderTargetWeakPtr targetWeak = it->second;
				if (!targetWeak.expired())
				{
					RenderTargetPtr target = targetWeak.lock();
					D3D9RenderTargetPtr d3d9Target = Util::checkedPtrCast<D3D9RenderTarget>(target);

					D3D9RenderTargetPtr newD3d9Target = Util::checkedPtrCast<D3D9RenderTarget>(this->createRenderTarget(targetMapping));
					d3d9Target->onDeviceReset(newD3d9Target);
				}
				else
				{
					mRenderTargetMappingMap.erase(it);
				}
			}
		}
	}

} /// end namespace