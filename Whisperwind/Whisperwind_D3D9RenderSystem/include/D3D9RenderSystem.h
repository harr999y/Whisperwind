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
#ifndef _D3D9_RENDER_SYSTEM_H_
#define _D3D9_RENDER_SYSTEM_H_

/** for windows.h's warning level */
#pragma warning(push, 3)
#include <windows.h>
#pragma warning(pop)

#include "EngineForwardDeclare.h"
#include "RenderSystem.h"
#include "D3D9ForwardDeclare.h"
#include "D3D9Typedefs.h"

namespace Engine
{
	class D3D9RenderSystem : public RenderSystem
	{
	public:
		D3D9RenderSystem();
		~D3D9RenderSystem();

	private:
		virtual void init_impl();
		virtual bool beginRendering_impl();
		virtual bool render_impl(const RenderablePtr & renderable);
		virtual void endRendering_impl();
		virtual void present_impl();
		virtual void clearFrame_impl(Util::u_int flag, Util::real zValue, Util::u_int stencilValue);
		virtual bool isPaused_impl();
		virtual RenderablePtr createRenderable_impl(const RenderableMappingPtr & rm);
		virtual RenderTexturePtr createRenderTexture_impl(const RenderTextureMappingPtr & rtm);
		virtual RenderTexturePtr createRenderTextureFromFile_impl(const Util::Wstring & path);
		virtual RenderTargetPtr createRenderTarget_impl(const RenderTargetMappingPtr & rtm);

		void createDevice(HWND window);
		void onDeviceLost();
		void onDeviceReset();
		bool reset();
		bool checkDeviceLostBeforeDraw();

	private:
		SET_GET_CONST_VALUE(bool, IsDeviceLost);

	private:
		HWND mWindow;
		IDirect3D9Ptr mD3D;
		IDirect3DDevice9Ptr mD3DDevice;
		D3D9CapabilityPtr mCapability;
		bool mIsDeviceLost;
		D3DPRESENT_PARAMETERS mPresentParameters;  /// Keep for device reset.
		ID3DXEffectMap mEffectMap;
		RenderableMappingWeakMap mRenderableMappingMap;
		RenderTextureMappingWeakMap mRenderTextureMappingMap;
		RenderTextureFileWeakMap mRenderTextureFileMap;
		RenderTargetMappingWeakMap mRenderTargetMappingMap;
	};
}

#endif