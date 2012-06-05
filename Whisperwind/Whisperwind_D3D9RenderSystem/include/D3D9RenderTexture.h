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
#ifndef _D3D9_RENDER_TEXTURE_H_
#define _D3D9_RENDER_TEXTURE_H_

#include "RenderTexture.h"
#include "D3D9ForwardDeclare.h"
#include "D3D9Typedefs.h"

namespace Engine
{
	class D3D9RenderTexture : public RenderTexture
	{
	public:
		D3D9RenderTexture()
		{}

		~D3D9RenderTexture();

	public:
		void onDeviceLost();
		void onDeviceReset(const D3D9RenderTexturePtr & newTexture);

	public:
		SET_GET_CONST_VALUE(IDirect3DTexture9Ptr, Texture);

	private:
		IDirect3DTexture9Ptr mTexture;
	};

}

#endif