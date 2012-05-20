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

#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Renderable.h"
#include "D3D9ForwardDeclare.h"
#include "D3D9Typedefs.h"

namespace Engine
{
	struct VertexBound
	{
		VertexBound() :
			VertexCount(0)
		{}

		IDirect3DVertexBuffer9PtrVector VertexBufferVec;
		VertexStrideVector VertexStrideVec;
		IDirect3DVertexDeclaration9Ptr VertexDeclaration;
		Util::u_int VertexCount;
	};

	struct EffectHandleSize
	{
		EffectHandleSize() :
			Handle(NULL),
			Size(0)
		{}

		D3DXHANDLE Handle;
		Util::u_int Size;
	};

	typedef std::pair<Util::String, VoidDataPtr> ParamValuePair;
	typedef std::vector<ParamValuePair> ParamValuePairVector;

	typedef std::pair<Util::String, D3D9RenderTexturePtr> ParamTexturePair;
	typedef std::vector<ParamTexturePair> ParamTexturePairVector;

	class D3D9Renderable : public Renderable, public boost::enable_shared_from_this<D3D9Renderable>
	{
	public:
		explicit D3D9Renderable(const IDirect3DDevice9Ptr & device) :
		    mD3DDevice(device),
		    mHasIndex(false),
			mTechnique(0),
			mPrimType(D3DPT_TRIANGLESTRIP)
		{}

		~D3D9Renderable()
		{}

	public:
		void onDeviceLost();
		void onDeviceReset(const D3D9RenderablePtr & newRenderable);

	public:
		SET_GET_VALUE(VertexBound, VertexBound);
		SET_GET_CONST_VALUE(IDirect3DIndexBuffer9Ptr, IndexBuffer);
		SET_GET_CONST_VALUE(ID3DXEffectPtr, Effect);
		SET_GET_CONST_VALUE(D3DXHANDLE, Technique);
		SET_GET_CONST_VALUE(bool, HasIndex);
		SET_GET_CONST_VALUE(D3DPRIMITIVETYPE, PrimType);
		SET_GET_CONST_VALUE(Util::u_int, PrimCount);
		SET_GET_VALUE(ParamValuePairVector, ParamValuePairVec);
		SET_GET_VALUE(ParamTexturePairVector, ParamTexturePairVec);

	private:
		virtual void setEffectSemanticValue_impl(const Util::String & semanticName, const void * data);
		virtual void setEffectParamValue_impl(const Util::String & paramName, const void * data);
		virtual void setTexture_impl(const Util::String & paramName, const RenderTexturePtr & texture);
		virtual void setRenderTarget_impl(Util::u_int index, const RenderTargetPtr & target);

		EffectHandleSize getEffectHandleSize(const Util::String & paramName);

	private:
		typedef boost::unordered_map<Util::String, EffectHandleSize> EffectHandleMap;

		IDirect3DDevice9Ptr mD3DDevice;

		VertexBound mVertexBound;
		IDirect3DIndexBuffer9Ptr mIndexBuffer;
		ID3DXEffectPtr mEffect;
		D3DXHANDLE mTechnique;
		bool mHasIndex;
		D3DPRIMITIVETYPE mPrimType;
		Util::u_int mPrimCount;
		EffectHandleMap mEffectHandleMap;
		ParamValuePairVector mParamValuePairVec;
		ParamTexturePairVector mParamTexturePairVec;
	};
}

#endif