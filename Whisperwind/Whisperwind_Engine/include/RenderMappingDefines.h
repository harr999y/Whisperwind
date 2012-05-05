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

#include "Util.h"
#include <vector>
#include "EngineForwardDeclare.h"
#include "boost/shared_ptr.hpp"

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

	struct WHISPERWIND_API VertexElement
	{
		VertexElement(Util::s_int16 streamIndex, Util::s_int16 offset, VertexElementType type, 
			VertexElementUsage usage, Util::s_int8 usageIndex) : 
		    StreamIndex(streamIndex), Offset(offset), Type(type), Usage(usage), UsageIndex(usageIndex)
		{}

		Util::s_int16 StreamIndex;
		Util::s_int16 Offset;
		VertexElementType Type;
		VertexElementUsage Usage;
		Util::s_int8 UsageIndex;
	};

	typedef std::vector<VertexElement> VertexElementVector;

	typedef boost::shared_ptr<void> VoidDataPtr;

	struct WHISPERWIND_API BufferData
	{
		BufferData() :
		    DataSize(0),
			Stride(0)
	    {}

	    VoidDataPtr Data;
		Util::u_int DataSize;
	    Util::u_int Stride;
	};

	struct WHISPERWIND_API VertexMapping
	{
		VertexMapping() :
	        VertexUsage(BUF_STATIC),
			VertexCount(0)
		{}

		VertexElementVector VertexElemVec;
		BufferData VertexData;
		BufferUsageFlag VertexUsage;
		Util::u_int VertexCount;
	};

	struct WHISPERWIND_API IndexMapping
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

	struct WHISPERWIND_API RenderableMapping
	{
		RenderableMapping() :
	        PrimCount(0),
			PrimType(PT_TRIANGLE_STRIP)
		{}

		VertexMapping VertexBound;
		IndexMapping IndexBound;
		Util::u_int PrimCount;
		Util::Wstring EffectName;
		Util::String TechniqueName;
		PrimitiveType PrimType;
	};

}

#endif