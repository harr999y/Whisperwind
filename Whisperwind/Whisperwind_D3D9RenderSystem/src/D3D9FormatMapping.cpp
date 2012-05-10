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

#include <boost/foreach.hpp>

#include "MakeCOMPtr.h"
#include "DebugDefine.h"
#include "RenderMappingDefines.h"
#include "D3D9FormatMapping.h"
#include "D3D9Helper.h"

namespace
{
	using namespace Engine;

	//---------------------------------------------------------------------
	// Helper functions.
	//---------------------------------------------------------------------
#define CASE_MATCH_RETURN(x, y) case x : return y;
#define CASE_UNMATCH_ASSERT default : WHISPERWIND_ASSERT(false);
	//---------------------------------------------------------------------
	D3DDECLTYPE translateVertexElementType(VertexElementType vet)
	{
		WHISPERWIND_ASSERT(vet < VET_MAX);

		switch (vet)
		{
			CASE_MATCH_RETURN(VET_FLOAT1, D3DDECLTYPE_FLOAT1);
			CASE_MATCH_RETURN(VET_FLOAT2, D3DDECLTYPE_FLOAT2);
			CASE_MATCH_RETURN(VET_FLOAT3, D3DDECLTYPE_FLOAT3);
			CASE_MATCH_RETURN(VET_FLOAT4, D3DDECLTYPE_FLOAT4);
			CASE_MATCH_RETURN(VET_COLOR, D3DDECLTYPE_D3DCOLOR);
			CASE_UNMATCH_ASSERT;
		}

		WHISPERWIND_ASSERT(false);
		return D3DDECLTYPE_UNUSED;
	}
	//---------------------------------------------------------------------
	D3DDECLUSAGE translateVertexElementUsage(VertexElementUsage veu)
	{
		WHISPERWIND_ASSERT(veu < VEU_MAX);

		switch (veu)
		{
			CASE_MATCH_RETURN(VEU_POSITION, D3DDECLUSAGE_POSITION);
			CASE_MATCH_RETURN(VEU_TEXTURE_COORD, D3DDECLUSAGE_TEXCOORD);
			CASE_MATCH_RETURN(VEU_NORMAL, D3DDECLUSAGE_NORMAL);
			CASE_MATCH_RETURN(VEU_TANGENT, D3DDECLUSAGE_TANGENT);
			CASE_MATCH_RETURN(VEU_COLOR, D3DDECLUSAGE_COLOR);
			CASE_UNMATCH_ASSERT;
		}

		WHISPERWIND_ASSERT(false);
		return D3DDECLUSAGE_TEXCOORD;
	}
	//---------------------------------------------------------------------
	Util::u_int translateClearFrameFlag(Util::u_int flag)
	{
		Util::u_int d3d9Flag = 0;

		if ((flag & FCF_TARGET) != 0)
			d3d9Flag |= D3DCLEAR_TARGET;
		if ((flag & FCF_ZBUFFER) != 0)
			d3d9Flag |= D3DCLEAR_ZBUFFER;
		if ((flag & FCF_STENCIL) != 0)
			d3d9Flag |= D3DCLEAR_STENCIL;

		return d3d9Flag;
	}
	//---------------------------------------------------------------------
	D3DPRIMITIVETYPE translatePrimitiveType(PrimitiveType primType)
	{
		WHISPERWIND_ASSERT(primType < PT_MAX);

		switch (primType)
		{
			CASE_MATCH_RETURN(PT_TRIANGLE_LIST, D3DPT_TRIANGLELIST);
			CASE_MATCH_RETURN(PT_TRIANGLE_STRIP, D3DPT_TRIANGLESTRIP);
			CASE_MATCH_RETURN(PT_LINE_LIST, D3DPT_LINELIST);
			CASE_UNMATCH_ASSERT;
		}

		WHISPERWIND_ASSERT(false);
		return D3DPT_FORCE_DWORD;
	}
	//---------------------------------------------------------------------
	Util::u_int translateTextureCreateFlag(Util::u_int flag)
	{
		Util::u_int d3d9Flag = 0;

		if ((flag & TCF_RENDERTARGET) != 0)
			d3d9Flag |= D3DUSAGE_RENDERTARGET;
		else if ((flag & TCF_DEPTHSTENCIL) != 0)
			d3d9Flag |= D3DUSAGE_DEPTHSTENCIL;

		if ((flag & TCF_AUTO_MIPMAP) != 0)
			d3d9Flag |= D3DUSAGE_AUTOGENMIPMAP;

		return d3d9Flag;
	}
	//---------------------------------------------------------------------
	D3DFORMAT translatePixelFormat(RenderPixelFormat fmt)
	{
		WHISPERWIND_ASSERT(fmt < RPF_MAX);

		switch (fmt)
		{
			CASE_MATCH_RETURN(RPF_A8R8G8B8, D3DFMT_A8R8G8B8);
			CASE_MATCH_RETURN(RPF_A16B16G16R16F, D3DFMT_A16B16G16R16F);
			CASE_MATCH_RETURN(RPF_R16F, D3DFMT_R16F);
			CASE_MATCH_RETURN(RPF_R32F, D3DFMT_R32F);
			CASE_MATCH_RETURN(RPF_D16, D3DFMT_D16);
			CASE_MATCH_RETURN(RPF_D32, D3DFMT_D32);
			CASE_MATCH_RETURN(RPF_D24X8, D3DFMT_D24X8);
			CASE_MATCH_RETURN(RPF_D24S8, D3DFMT_D24S8);
			CASE_UNMATCH_ASSERT;
		}

		WHISPERWIND_ASSERT(false);
		return D3DFMT_UNKNOWN;
	}
	//---------------------------------------------------------------------
#undef CASE_MATCH_RETURN
#undef CASE_UNMATCH_ASSERT
}

namespace Engine
{
	//---------------------------------------------------------------------
	// D3D9FormatMappingFactory
	//---------------------------------------------------------------------
	IDirect3DVertexDeclaration9Ptr D3D9FormatMappingFactory::createD3D9VertexDeclaration(const IDirect3DDevice9Ptr & device, const VertexElementVector & veVec)
	{
		D3DVERTEXELEMENT9Vector d3dVertexElemVec;
		D3DVERTEXELEMENT9 d3dVertexElem;

		BOOST_FOREACH(const VertexElement & ve, veVec)
		{
			d3dVertexElem.Method = D3DDECLMETHOD_DEFAULT;
			d3dVertexElem.Offset = ve.Offset;
			d3dVertexElem.Stream = ve.StreamIndex;
			d3dVertexElem.Type = static_cast<BYTE>(translateVertexElementType(ve.Type));
			d3dVertexElem.Usage = static_cast<BYTE>(translateVertexElementUsage(ve.Usage));
			d3dVertexElem.UsageIndex = ve.UsageIndex;

			d3dVertexElemVec.push_back(d3dVertexElem);
		}
		{
			d3dVertexElem.Stream = 0xFF;
			d3dVertexElem.Offset = 0;
			d3dVertexElem.Type = D3DDECLTYPE_UNUSED;
			d3dVertexElem.Method = 0;
			d3dVertexElem.Usage = 0;
			d3dVertexElem.UsageIndex = 0;

			d3dVertexElemVec.push_back(d3dVertexElem);
		}

		IDirect3DVertexDeclaration9 * vertexDeclaration = NULL;
		DX_IF_FAILED_DEBUG_PRINT(device->CreateVertexDeclaration(&d3dVertexElemVec[0], &vertexDeclaration));

		return Util::makeCOMPtr(vertexDeclaration);
	}
	//---------------------------------------------------------------------
	Util::u_int D3D9FormatMappingFactory::getD3D9ClearFrameFlag(Util::u_int flag)
	{
		return translateClearFrameFlag(flag);
	}
	//---------------------------------------------------------------------
	D3DPRIMITIVETYPE D3D9FormatMappingFactory::getD3D9PrimType(PrimitiveType primType)
	{
		return translatePrimitiveType(primType);
	}
	//---------------------------------------------------------------------
	Util::u_int D3D9FormatMappingFactory::getD3D9TextureCreateFlag(Util::u_int flag)
	{
		return translateTextureCreateFlag(flag);
	}
	//---------------------------------------------------------------------
	D3DFORMAT D3D9FormatMappingFactory::getD3D9PixelFormat(RenderPixelFormat fmt)
	{
		return translatePixelFormat(fmt);
	}
}