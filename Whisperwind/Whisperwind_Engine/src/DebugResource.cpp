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

#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include "DebugDefine.h"
#include "AABB.h"
#include "EngineManager.h"
#include "SceneManager.h"
#include "SceneNode.h"
#include "SceneObject.h"
#include "RenderMappingDefines.h"
#include "DebugResource.h"

namespace Engine
{
	static const Util::String EFFECT_NAME("DebugAABB.fx");
	static const Util::String NODE_TECHNIQUE("RedTech");
	static const Util::String OBJECT_TECHNIQUE("YellowTech");
	static const Util::String RENDERABLE_TECHNIQUE("GreenTech");
	static const Util::Wstring AUTO_RENDERABLE_NAME_SUFFIX(TO_UNICODE("_AABBRenderable"));
	//---------------------------------------------------------------------
	const Util::Wstring DebugResource::mDebugCreateType(TO_UNICODE("debug"));
	const Util::Wstring DebugResource::mDebugNodeSuffix(TO_UNICODE(".debug_node"));
	const Util::Wstring DebugResource::mDebugObjectSuffix(TO_UNICODE(".debug_object"));
	//---------------------------------------------------------------------
	void DebugResource::load(const Util::Wstring & name)
	{
		const SceneManagerPtr & sceneMgr = EngineManager::getSingleton().getSceneManager();
		Util::AABBPtr aabb;

		if (boost::algorithm::find_first(name, mDebugNodeSuffix))
		{
			Util::Wstring nodeName = name;
			boost::algorithm::erase_first(nodeName, mDebugNodeSuffix);

			const SceneNodePtr & node = sceneMgr->getSceneNode(nodeName);
			WHISPERWIND_ASSERT(node);

			aabb = node->getAABB();

			constructRenderableMapping(aabb, DT_NODE);
		}
		else if (boost::algorithm::find_first(name, mDebugObjectSuffix))
		{
			Util::Wstring objName = name;
			boost::algorithm::erase_first(objName, mDebugObjectSuffix);

			const SceneNodePtr & node = sceneMgr->getSceneNode(objName);
			WHISPERWIND_ASSERT(node);

			aabb = node->getAABB();

			constructRenderableMapping(aabb, DT_OBJECT);
		}
	}
	//---------------------------------------------------------------------
	void DebugResource::constructRenderableMapping(const Util::AABBPtr & aabb, DebugType type)
	{
		RenderableMappingPtr rm = boost::make_shared<RenderableMapping>();

		/// VertexMapping
		{
			VertexMapping & vm = rm->VertexBound;

			vm.VertexCount = 8;

			VertexElement ve(0, 0, VET_FLOAT3, VEU_POSITION, 0);
			vm.VertexElemVec.push_back(ve);

			BufferData bd;
			bd.DataSize = 96;
			bd.Stride = 12;
			Uint8Vector & vec = bd.DataVec;
			vec.resize(bd.DataSize);
			Util::real * data = reinterpret_cast<Util::real *>(vec.data());
			{
				XMFLOAT3 minPoint;
				XMStoreFloat3(&minPoint, aabb->getMinPoint());

				XMFLOAT3 maxPoint;
				XMStoreFloat3(&maxPoint, aabb->getMaxPoint());

				/// 1
				*(data++) = minPoint.x;
				*(data++) = minPoint.y;
				*(data++) = minPoint.z;

				/// 2
				*(data++) = minPoint.x;
				*(data++) = maxPoint.y;
				*(data++) = minPoint.z;

				/// 3
				*(data++) = maxPoint.x;
				*(data++) = minPoint.y;
				*(data++) = minPoint.z;

				/// 4
				*(data++) = maxPoint.x;
				*(data++) = maxPoint.y;
				*(data++) = minPoint.z;

				/// 5
				*(data++) = maxPoint.x;
				*(data++) = minPoint.y;
				*(data++) = maxPoint.z;

				/// 6
				*(data++) = maxPoint.x;
				*(data++) = maxPoint.y;
				*(data++) = maxPoint.z;

				/// 7
				*(data++) = minPoint.x;
				*(data++) = minPoint.y;
				*(data++) = maxPoint.z;

				/// 8
				*(data++) = minPoint.x;
				*(data++) = maxPoint.y;
				*(data) = maxPoint.z;
			}
			vm.VertexDataVec.push_back(bd);
		}
		/// IndexMapping
		{
			IndexMapping & im = rm->IndexBound;

			im.HasIndex = true;

			BufferData & bd = im.IndexData;
			bd.DataSize = 48;
			bd.Stride = 2;
			Uint8Vector &vec = bd.DataVec;
			vec.resize(bd.DataSize);
			Util::u_int16 * data = reinterpret_cast<Util::u_int16 *>(vec.data());
			{
				*(data++) = 0;*(data++) = 1;
				*(data++) = 0;*(data++) = 2;
				*(data++) = 0;*(data++) = 6;
				*(data++) = 1;*(data++) = 3;
				*(data++) = 1;*(data++) = 7;
				*(data++) = 2;*(data++) = 3;
				*(data++) = 2;*(data++) = 4;
				*(data++) = 3;*(data++) = 5;
				*(data++) = 4;*(data++) = 5;
				*(data++) = 4;*(data++) = 6;
				*(data++) = 5;*(data++) = 7;
				*(data++) = 6;*(data) = 7;
			}
		}
		/// Effect/Technique
		{
			rm->EffectName = EFFECT_NAME;

			switch (type)
			{
			case DT_NODE:
				{
					rm->TechniqueName = NODE_TECHNIQUE;
					break;
				}
			case DT_OBJECT:
				{
					rm->TechniqueName = OBJECT_TECHNIQUE;
					break;
				}
			case DT_RENDERABLE:
				{
					rm->TechniqueName = RENDERABLE_TECHNIQUE;
					break;
				}
			}
		}
		/// Prim
		{
			rm->PrimCount = 12;
			rm->PrimType = PT_LINE_LIST;
		}

		/// name
		{
			static Util::u_int autoNameSuffixNum = 0;

			rm->RenderableName = boost::lexical_cast<Util::Wstring>(autoNameSuffixNum) + AUTO_RENDERABLE_NAME_SUFFIX;
		}

		mRenderableMapping = rm;
	}
	//---------------------------------------------------------------------
	const Util::Wstring & DebugResource::getDebugCreateType()
	{
		return mDebugCreateType;
	}
	//---------------------------------------------------------------------
	const Util::Wstring & DebugResource::getDebugNodeSuffix()
	{
		return mDebugNodeSuffix;
	}
	//---------------------------------------------------------------------
	const Util::Wstring & DebugResource::getDebugObjectSuffix()
	{
		return mDebugObjectSuffix;
	}

}