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
#ifndef _D3D9_FORMAT_MAPPING_H_
#define _D3D9_FORMAT_MAPPING_H_

#include "EngineForwardDeclare.h"
#include "RenderMappingDefines.h"
#include "D3D9Typedefs.h"

namespace Engine
{
	class D3D9FormatMappingFactory
	{
	public:
		static IDirect3DVertexDeclaration9Ptr createD3D9VertexDeclaration(const IDirect3DDevice9Ptr & device, const VertexElementVector & veVec);
		static Util::u_int getD3D9ClearFrameFlag(Util::u_int flag);
		static D3DPRIMITIVETYPE getD3D9PrimType(PrimitiveType primType);
		static Util::u_int getD3D9TextureCreateFlag(Util::u_int flag);
		static D3DFORMAT getD3D9PixelFormat(RenderPixelFormat fmt);

	private:
		MAKE_STATIC_CLASS(D3D9FormatMappingFactory);
	};
}

#endif