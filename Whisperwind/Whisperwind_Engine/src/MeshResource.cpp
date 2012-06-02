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

#include <fstream>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

#include "AABB.h"
#include "StringConverter.h"
#include "EngineSerialization_impl.h"
#include "Mesh.h"
#include "EngineManager.h"
#include "EngineConfig.h"
#include "RenderMappingDefines.h"
#include "MeshResource.h"

namespace Engine
{
	//---------------------------------------------------------------------
	void MeshResource::load(const Util::Wstring & resourcePath)
	{
		std::ifstream ifs(Util::WstringToString(resourcePath), std::ios::binary);
		boost::archive::binary_iarchive is(ifs);

		is >> BOOST_SERIALIZATION_NVP(mMesh);

		/// if need to render normal.
		if (EngineManager::getSingleton().getEngineConfig()->getDebugNormal())
		{
			processNormalDebug();
		}
	}
	//---------------------------------------------------------------------
	void MeshResource::processNormalDebug()
	{
		IF_FALSE_RETURN(!mMesh->getSubMeshVec().empty());

		SubMeshPtr subMesh = boost::make_shared<SubMesh>();
		RenderableMappingPtr rm = boost::make_shared<RenderableMapping>();
		subMesh->setRenderableMapping(rm);

		rm->RenderableName = (mMesh->getSubMeshVec())[0]->getRenderableMapping()->RenderableName + 
			TO_UNICODE("_normal_debug");
		rm->EffectName = "DebugNormal.fx";
		rm->TechniqueName = "YellowTech";
		rm->IndexBound.HasIndex = false;
		rm->PrimType = PT_LINE_LIST;
		rm->VertexBound.VertexUsage = BUF_STATIC;

		VertexElement vertexElem(0, 0, Engine::VET_FLOAT3, Engine::VEU_POSITION, 0);
		rm->VertexBound.VertexElemVec.push_back(vertexElem);

		SubMeshVector & smVec = mMesh->getSubMeshVec();
		BOOST_FOREACH(const SubMeshPtr & sm, smVec)
		{
			const RenderableMappingPtr & oldRm = sm->getRenderableMapping();
			rm->VertexBound.VertexCount += oldRm->VertexBound.VertexCount * 2;
		}

		rm->PrimCount = rm->VertexBound.VertexCount / 2;

		Engine::Uint8Vector dataVec((sizeof(Util::real) / sizeof(Engine::Uint8Vector::value_type)) * rm->VertexBound.VertexCount * 3, 0);
		Util::real * data = reinterpret_cast<Util::real *>(dataVec.data());
		BOOST_FOREACH(const SubMeshPtr & sm, smVec)
		{
			const RenderableMappingPtr & oldRm = sm->getRenderableMapping();
			const BufferData & oldBufData = oldRm->VertexBound.VertexDataVec[0];

			const Util::real * oldData = reinterpret_cast<const Util::real *>(oldBufData.DataVec.data());

			Util::u_int oldVertexSize = oldBufData.Stride / sizeof(Util::real);
			for (Util::u_int vertexIt = 0; vertexIt < oldRm->VertexBound.VertexCount; ++vertexIt)
			{
				/// first vertex
				*(data++) = *(oldData + 0);
				*(data++) = *(oldData + 1);
				*(data++) = *(oldData + 2);

				/// second vertex
				*(data++) = *(oldData + 0) + *(oldData + 3) * 10.0f;
				*(data++) = *(oldData + 1) + *(oldData + 4) * 10.0f;
				*(data++) = *(oldData + 2) + *(oldData + 5) * 10.0f;

				oldData += oldVertexSize;
			}
		}

		BufferData bufData;
		bufData.DataSize = sizeof(Util::real) * rm->VertexBound.VertexCount * 3;
		bufData.DataVec = dataVec;
		bufData.Stride = sizeof(Util::real) * 3;

		rm->VertexBound.VertexDataVec.push_back(bufData);

		smVec.push_back(subMesh);
	}

}