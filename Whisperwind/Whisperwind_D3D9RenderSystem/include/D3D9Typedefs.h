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
#ifndef _D3D9_TYPEDEFS_H_
#define _D3D9_TYPEDEFS_H_

#include <vector>
#include <d3d9.h>
#include <d3dx9.h>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "Util.h"

#ifdef WHISPERWIND_DEBUG
#pragma comment(lib, "d3dx9d.lib")
#else
#pragma comment(lib, "d3dx9.lib")
#endif

namespace Engine
{
	typedef boost::shared_ptr<IDirect3D9> IDirect3D9Ptr;
	typedef boost::shared_ptr<IDirect3DDevice9> IDirect3DDevice9Ptr;
	typedef boost::shared_ptr<IDirect3DSwapChain9> IDirect3DSwapChain9Ptr;
	typedef boost::shared_ptr<IDirect3DResource9> IDirect3DResource9Ptr;
	typedef boost::shared_ptr<IDirect3DCubeTexture9> IDirect3DCubeTexture9Ptr;
	typedef boost::shared_ptr<IDirect3DTexture9> IDirect3DTexture9Ptr;
	typedef boost::shared_ptr<ID3DXEffect> ID3DXEffectPtr;
	typedef boost::shared_ptr<IDirect3DSurface9> IDirect3DSurface9Ptr;
	typedef boost::shared_ptr<IDirect3DVertexDeclaration9> IDirect3DVertexDeclaration9Ptr;
	typedef boost::shared_ptr<IDirect3DVertexBuffer9> IDirect3DVertexBuffer9Ptr;
	typedef boost::shared_ptr<IDirect3DIndexBuffer9> IDirect3DIndexBuffer9Ptr;
	
	typedef std::vector<D3DVERTEXELEMENT9> D3DVERTEXELEMENT9Vector;

	typedef boost::unordered_map<Util::Wstring, ID3DXEffectPtr> ID3DXEffectMap;
}

#endif