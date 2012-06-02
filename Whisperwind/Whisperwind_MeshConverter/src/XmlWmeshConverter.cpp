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

#include <istream>
#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>

#include "ExceptionDefine.h"
#include "StringConverter.h"
#include "AABB.h"
#include "EngineSerialization_impl.h"
#include "RenderMappingDefines.h"
#include "Mesh.h"
#include "XmlWmeshConverter.h"

namespace Tool
{
	//---------------------------------------------------------------------
	XmlWmeshConverter::XmlWmeshConverter(const Util::String & path) :
        mPath(path)
	{
		mXmlReader = boost::make_shared<Util::XmlReader>(path);
	}
	//---------------------------------------------------------------------
	void XmlWmeshConverter::converteToWmesh()
	{
		Util::XmlNode * rootNode = mXmlReader->getRootNode();
		IF_NULL_EXCEPTION(rootNode, (mPath + " donnot have root node!").c_str());

		Util::XmlNode * meshNode = mXmlReader->getFirstNode(rootNode, "mesh");
		IF_NULL_EXCEPTION(rootNode, (mPath + " donnot have mesh node!").c_str());

		Engine::MeshPtr mesh = boost::make_shared<Engine::Mesh>();
		{
			Util::AABBPtr meshAABB = boost::make_shared<Util::AABB>();

			Engine::SubMeshVector smVec;

			Util::XmlNode * submeshNode = mXmlReader->getFirstNode(meshNode, "submesh");
			while (submeshNode)
			{
				Engine::SubMeshPtr sm = boost::make_shared<Engine::SubMesh>();

				doConvert(submeshNode, sm);
				smVec.push_back(sm);

				meshAABB->merge(sm->getAABB());

				submeshNode = mXmlReader->getNextSiblingNode(submeshNode);
			}

			mesh->setSubMeshVec(smVec);
			mesh->setAABB(meshAABB);
		}

		boost::algorithm::erase_last(mPath, ".xml");
 		std::ofstream ofs(mPath, std::ios::out | std::ios::trunc | std::ios::binary);
 		boost::archive::binary_oarchive oa(ofs);
 		oa << BOOST_SERIALIZATION_NVP(mesh);
		ofs.close();
	}
	//---------------------------------------------------------------------
	void XmlWmeshConverter::doConvert(const Util::XmlNode * submeshNode, Engine::SubMeshPtr & sm)
	{
		Engine::RenderableMappingPtr rm = boost::make_shared<Engine::RenderableMapping>();

		rm->RenderableName = Util::StringToWstring(mXmlReader->getAttribute(submeshNode, "name"));

		Util::XmlNode * materialNode = mXmlReader->getFirstNode(submeshNode, "material");
		if (materialNode)
		{
			rm->EffectName = mXmlReader->getAttribute(materialNode, "effect");
			rm->TechniqueName = mXmlReader->getAttribute(materialNode, "technique");

			Engine::EffectParamValuePair pvPair;
			{
				Util::XmlNode * paramNode = mXmlReader->getFirstNode(materialNode, "param");
				while (paramNode)
				{
					pvPair.first = mXmlReader->getAttribute(paramNode, "name");
					pvPair.second = mXmlReader->getAttribute(paramNode, "value");
					rm->ParamValueVec.push_back(pvPair);

					paramNode = mXmlReader->getNextSiblingNode(paramNode);
				}

				Util::XmlNode * textureNode = mXmlReader->getFirstNode(materialNode, "texture");
				while (textureNode)
				{
					pvPair.first = mXmlReader->getAttribute(textureNode, "name");
					pvPair.second = mXmlReader->getAttribute(textureNode, "value");
					rm->ParamTextureVec.push_back(pvPair);

					textureNode = mXmlReader->getNextSiblingNode(textureNode);
				}
			}
		}

		Util::XmlNode * vbNode = mXmlReader->getFirstNode(submeshNode, "vertexbuffer");
		IF_NULL_EXCEPTION(vbNode, mPath + " donnot have vertexbuffer!");
		{
			Util::u_int vertexCount = boost::lexical_cast<Util::u_int>(mXmlReader->getAttribute(vbNode, "vertexcount"));
			rm->VertexBound.VertexCount = vertexCount;

			Util::XmlNode * vertexNode = mXmlReader->getFirstNode(vbNode, "vertex");
			IF_NULL_EXCEPTION(vertexNode, mPath + " donnot have vertex!");

			Util::s_int16 vertexSize = 0;
			{
				/// NOTE:Just now what I need is only position,normal and texcoord,so I write just like this.
				if (mXmlReader->getFirstNode(vertexNode, "position"))
				{
					Engine::VertexElement vertexElem(0, 0, Engine::VET_FLOAT3, Engine::VEU_POSITION, 0);
					rm->VertexBound.VertexElemVec.push_back(vertexElem);

					vertexSize += 3;
				}
				if (mXmlReader->getFirstNode(vertexNode, "normal"))
				{
					Engine::VertexElement vertexElem(0, vertexSize * 4, Engine::VET_FLOAT3, Engine::VEU_NORMAL, 0);
					rm->VertexBound.VertexElemVec.push_back(vertexElem);

					vertexSize += 3;
				}
				if (mXmlReader->getFirstNode(vertexNode, "texcoord"))
				{
					Engine::VertexElement vertexElem(0, vertexSize * 4, Engine::VET_FLOAT2, Engine::VEU_TEXTURE_COORD, 0);
					rm->VertexBound.VertexElemVec.push_back(vertexElem);

					vertexSize += 2;
				}
			}

			Engine::Uint8Vector dataVec((sizeof(Util::real) / sizeof(Engine::Uint8Vector::value_type)) * vertexSize * vertexCount, 0);
			Util::real * data = reinterpret_cast<Util::real *>(dataVec.data());
			Util::AABBPtr aabb = boost::make_shared<Util::AABB>();
			do 
			{
				/// position
				Util::XmlNode * positionNode = mXmlReader->getFirstNode(vertexNode, "position");
				if (positionNode)
				{
					Util::real x = boost::lexical_cast<Util::real>(mXmlReader->getAttribute(positionNode, "x"));
					Util::real y = boost::lexical_cast<Util::real>(mXmlReader->getAttribute(positionNode, "y"));
					Util::real z = boost::lexical_cast<Util::real>(mXmlReader->getAttribute(positionNode, "z"));

					*(data++) = x;
					*(data++) = y;
					*(data++) = z;

					/// do calc subMesh's AABB here.
					{
						aabb->addPoint(XMVectorSet(x, y, z, 0.0f));
					}
				}

				/// normal
				Util::XmlNode * normalNode = mXmlReader->getFirstNode(vertexNode, "normal");
				if (normalNode)
				{
					*(data++) = boost::lexical_cast<Util::real>(mXmlReader->getAttribute(normalNode, "x"));
					*(data++) = boost::lexical_cast<Util::real>(mXmlReader->getAttribute(normalNode, "y"));
					*(data++) = boost::lexical_cast<Util::real>(mXmlReader->getAttribute(normalNode, "z"));
				}

				/// texcoord
				Util::XmlNode * texcoordNode = mXmlReader->getFirstNode(vertexNode, "texcoord");
				if (texcoordNode)
				{
					*(data++) = boost::lexical_cast<Util::real>(mXmlReader->getAttribute(texcoordNode, "u"));
					*(data++) = boost::lexical_cast<Util::real>(mXmlReader->getAttribute(texcoordNode, "v"));
				}

				vertexNode = mXmlReader->getNextSiblingNode(vertexNode);
			} while (vertexNode);

			Engine::BufferData vbBufData;
			vbBufData.DataSize = sizeof(Util::real) * vertexSize * vertexCount;
			vbBufData.DataVec = dataVec;
			vbBufData.Stride = sizeof(Util::real) * vertexSize;
			rm->VertexBound.VertexDataVec.push_back(vbBufData);

			rm->VertexBound.VertexUsage = Engine::BUF_STATIC;

			sm->setAABB(aabb);
		}

		Util::XmlNode * trianglesNode = mXmlReader->getFirstNode(submeshNode, "triangles");
		if (trianglesNode)
		{
			Util::u_int triangleCount = boost::lexical_cast<Util::u_int>(mXmlReader->getAttribute(trianglesNode, "trianglescount"));
			if (triangleCount > 0)
			{
				Util::u_int indexCount = triangleCount * 3;
				if (indexCount < 65536)
				{
					rm->IndexBound = getIndexMapping<Util::u_int16>(trianglesNode, indexCount);

					rm->IndexBound.IndexFmt = Engine::INDEX_16;
				}
				else
				{
					rm->IndexBound = getIndexMapping<Util::u_int>(trianglesNode, indexCount);

					rm->IndexBound.IndexFmt = Engine::INDEX_32;
				}
			}

			rm->PrimCount = triangleCount;
			rm->IndexBound.IndexUsage = Engine::BUF_STATIC;
			rm->PrimType = Engine::PT_TRIANGLE_LIST;
		}

		sm->setRenderableMapping(rm);
	}

}