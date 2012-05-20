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
#ifndef _RENDER_MAPPING_DEFINES_H_
#define _RENDER_MAPPING_DEFINES_H_

#include <vector>
#include <utility>
#include <boost/shared_ptr.hpp>

#include "Util.h"
#include "EngineForwardDeclare.h"

namespace Engine
{
	enum VertexElementType
	{
		VET_FLOAT1,
		VET_FLOAT2,
		VET_FLOAT3,
		VET_FLOAT4,
		VET_COLOR,
		VET_MAX
	};

	enum VertexElementUsage
	{
		VEU_POSITION,
		VEU_TEXTURE_COORD,
		VEU_NORMAL,
		VEU_TANGENT,
		VEU_COLOR,
		VEU_MAX
	};

	enum PrimitiveType
	{
		PT_TRIANGLE_LIST,
		PT_TRIANGLE_STRIP,
		PT_LINE_LIST,
		PT_MAX
	};

	enum IndexFormat
	{
		INDEX_16,
		INDEX_32
	};

	enum BufferUsageFlag
	{
		BUF_STATIC,
		BUF_DYNAMIC
	};

	enum FrameClearFlag
	{
		FCF_TARGET = 1 << 0,
		FCF_ZBUFFER = 1 << 1,
		FCF_STENCIL = 1 << 2
	};

	enum TextureType
	{
		TT_1D,
		TT_2D,
		TT_CUBE
	};

	enum RenderPixelFormat
	{
		RPF_A8R8G8B8,
		RPF_A16B16G16R16F,
		RPF_R16F,
		RPF_R32F,
		RPF_D16,
		RPF_D32,
		RPF_D24X8,
		RPF_D24S8,
		RPF_MAX
	};

	enum TextureCreateFlag
	{
		TCF_RENDERTARGET = 1 << 0,
		TCF_DEPTHSTENCIL = 2 << 0,
		TCF_AUTO_MIPMAP = 3 << 0,
	};

	enum RenderType
	{
		RT_OPAQUE,
		RT_TRANSPARENT
	};

	struct VertexElement
	{
		VertexElement(Util::s_int16 streamIndex, Util::s_int16 offset, VertexElementType type, 
			VertexElementUsage usage, Util::s_int8 usageIndex) : 
		    StreamIndex(streamIndex), Offset(offset), Type(type), Usage(usage), UsageIndex(usageIndex)
		{}

		VertexElement()
		{}

		Util::s_int16 StreamIndex;
		Util::s_int16 Offset;
		VertexElementType Type;
		VertexElementUsage Usage;
		Util::s_int8 UsageIndex;
	};

	typedef std::vector<VertexElement> VertexElementVector;

	/// Use it carefully!
	typedef boost::shared_ptr<void> VoidDataPtr;

	typedef std::vector<Util::u_int8> Uint8Vector;

	struct BufferData
	{
		BufferData() :
		    DataSize(0),
			Stride(0)
	    {}

	    Uint8Vector DataVec;
		Util::u_int DataSize;
	    Util::u_int Stride;
	};

	typedef std::vector<BufferData> BufferDataVector;

	struct VertexMapping
	{
		VertexMapping() :
	        VertexUsage(BUF_STATIC),
			VertexCount(0)
		{}

		VertexElementVector VertexElemVec;
		BufferDataVector VertexDataVec;
		BufferUsageFlag VertexUsage;
		Util::u_int VertexCount;
	};

	struct IndexMapping
	{
		IndexMapping() :
	        HasIndex(false),
			IndexFmt(INDEX_16),
			IndexUsage(BUF_STATIC)
		{}

		bool HasIndex;
		BufferData IndexData;
		IndexFormat IndexFmt;
		BufferUsageFlag IndexUsage;
	};

	typedef std::pair<Util::String, Util::String> EffectParamValuePair;
	typedef std::vector<EffectParamValuePair> EffectParamValuePairVector;

	struct RenderableMapping
	{
		RenderableMapping() :
			PrimType(PT_TRIANGLE_STRIP),
			RenderingType(RT_OPAQUE),
			PrimCount(0)
		{}

		VertexMapping VertexBound;
		IndexMapping IndexBound;
		Util::String EffectName;
		Util::String TechniqueName;
		PrimitiveType PrimType;
		Util::u_int PrimCount;
		RenderType RenderingType;
		EffectParamValuePairVector ParamTextureVec;
		EffectParamValuePairVector ParamValueVec;
	};

	struct RenderTextureMapping
	{
		RenderTextureMapping() :
	        Levels(1),
			Type(TT_2D)
		{}

		Util::u_int Width;
		Util::u_int Height;
		Util::u_int Levels;
		TextureType Type;
		TextureCreateFlag Usage;
		RenderPixelFormat Format;
	};

	struct RenderTargetMapping
	{
		RenderTargetMapping() :
	        MultiSampleType(0),
			MultiSampleQuality(0)
		{}

		Util::u_int Width;
		Util::u_int Height;
		RenderPixelFormat Format;
		Util::u_int MultiSampleType;
		Util::u_int MultiSampleQuality;
	};

}

#endif