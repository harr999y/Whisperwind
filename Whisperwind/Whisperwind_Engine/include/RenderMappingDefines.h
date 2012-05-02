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

namespace Engine
{
	enum SamplerFilter
	{
		SF_POINT,
		SF_BILINEAR,
		SF_TRILINEAR,
		SF_ANISOTROPIC_POINT,
		SF_ANISOTROPIC_LINER,
		SF_MAX
	};

	enum SamplerAddressMode
	{
		SAM_WRAP,
		SAM_CLAMP,
		SAM_MIRROR,
		SAM_MAX
	};

	enum FillMode
	{
		FM_POINT,
		FM_WIREFRAME,
		FM_SOLID,
		FM_MAX
	};

/// NOTE:For what defined in WinGDI.h.I will never use it,so just undef.
#undef CM_NONE
	enum CullMode
	{
		CM_NONE,
		CM_CW,
		CM_CCW,
		CM_MAX
	};

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

	enum CompareFunction
	{
		CF_LESS,
		CF_LESS_EQUAL,
		CF_GREATER,
		CF_GREATER_EQUAL,
		CF_EQUAL,
		CF_NOT_EQUAL,
		CF_NEVER,
		CF_ALWAYS
	};

	enum PrimitiveType
	{
		PT_TRIANGLE_LIST,
		PT_TRIANGLE_STRIP,
		PT_LINE_LIST,
		PT_MAX
	};

	struct VertexElement
	{
		Util::s_int16 StreamIndex;
		Util::s_int16 Offset;
		VertexElementType Type;
		VertexElementUsage Usage;
		Util::s_int8 UsageIndex;
	};

	typedef Util::u_int16 IndexFormat;
	typedef std::vector<VertexElement> VertexElementVector;


	typedef std::vector<IndexFormat> IndexVector;
}

#endif