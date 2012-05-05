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
#ifndef _D3D9_RENDERABLE_H_
#define _D3D9_RENDERABLE_H_

#include "Renderable.h"
#include "D3D9Typedefs.h"
#include "boost/unordered_map.hpp"

namespace Engine
{
	struct VertexBound
	{
		VertexBound() :
	        VertexBufSize(0),
			VertexStride(0),
			VertexCount(0)
		{}

		IDirect3DVertexDeclaration9Ptr VertexDeclaration;
		IDirect3DVertexBuffer9Ptr VertexBuffer;
		Util::u_int VertexBufSize;
		Util::u_int VertexStride;
		Util::u_int VertexCount;
	};

	class D3D9Renderable : public Renderable
	{
	public:
		D3D9Renderable() : 
		    mHasIndex(false),
			mTechnique(0),
			mPrimCount(0),
			mPrimType(D3DPT_TRIANGLESTRIP)
		{}

	public:
		SET_GET_CONST_VALUE(VertexBound, VertexBound);
		SET_GET_CONST_VALUE(IDirect3DIndexBuffer9Ptr, IndexBuffer);
		SET_GET_CONST_VALUE(ID3DXEffectPtr, Effect);
		SET_GET_CONST_VALUE(D3DXHANDLE, Technique);
		SET_GET_CONST_VALUE(bool, HasIndex);
		SET_GET_CONST_VALUE(Util::u_int, PrimCount);
		SET_GET_CONST_VALUE(D3DPRIMITIVETYPE, PrimType);

	private:
		virtual void setEffectParamValue_impl(const Util::String & paramName, const void * data);

	private:
		struct EffectParamSize
		{
			EffectParamSize() :
		        Handle(NULL),
				Size(0)
			{}

			D3DXHANDLE Handle;
			Util::u_int Size;
		};
		typedef boost::unordered_map<Util::String, EffectParamSize> EffectParamMap;

		VertexBound mVertexBound;
		IDirect3DIndexBuffer9Ptr mIndexBuffer;
		ID3DXEffectPtr mEffect;
		D3DXHANDLE mTechnique;
		bool mHasIndex;
		Util::u_int mPrimCount;
		D3DPRIMITIVETYPE mPrimType;
		EffectParamMap mEffectParamMap;
	};
}

#endif