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
#ifndef _LOOSE_OCTREE_H_
#define _LOOSE_OCTREE_H_

#include <boost/enable_shared_from_this.hpp>

#include "Util.h"
#include "MathDefine.h"
#include "EngineForwardDeclare.h"
#include "GeneralForwardDeclare.h"

namespace Engine
{
	class LooseOctree
	{
	public:
		LooseOctree();

		~LooseOctree()
		{}

		void addOctreeNode(const OctreeSceneNodePtr & node);
		void removeOctreeNode(const SceneNodePtr & node);
		void removeAllOctreeNode();

		bool isInside(const Util::AABBPtr & aabb);
		const SceneNodeVector & findVisibles(const FrustumPtr & frustum);

		void createDebugRendering();

	private:
		LooseOctreeZonePtr mRootZone;
		SceneNodeVector mVisibleNodeVec;

	private:
		DISALLOW_COPY_AND_ASSIGN(LooseOctree);
	};

	class LooseOctreeZone : public boost::enable_shared_from_this<LooseOctreeZone>
	{
	public:
		LooseOctreeZone(FXMVECTOR minPoint, FXMVECTOR maxPoint, Util::u_int depth);

		~LooseOctreeZone()
		{}

	public:
		void addOctreeNode(const OctreeSceneNodePtr & node, Util::u_int depth);
		void removeOctreeNode(const SceneNodePtr & node);
		void removeAllOctreeNode();
		void clear();

		bool isEmpty() { return mTotalNodes <= 0; }
		bool isInside(const Util::AABBPtr & aabb);
		const Util::AABBPtr & getCullAABB();

		void createDebugRendering();

	public:
		GET_CONST_VALUE(Util::AABBPtr, AABB);
		GET_CONST_VALUE(SceneNodeVector, SceneNodeVec);

	private:
		bool isFitInChildZone(const Util::AABBPtr & aabb);
		LooseOctreeZonePtr & getFitChildZone(const Util::AABBPtr & aabb);

		friend const SceneNodeVector & LooseOctree::findVisibles(const FrustumPtr & frustum);
		void walkOctree(const FrustumPtr & frustum, SceneNodeVector & outNodeVec);

		inline void increaseTotalNodesCount();
		inline void decreaseTotalNodesCount();

	private:
		SET_GET_CONST_VALUE(LooseOctreeZonePtr, ParentZone);

	private:
		Util::AABBPtr mAABB;
		Util::u_int mDepth;
		Util::u_int mTotalNodes; /// Include all child zones' nodes.
		SceneNodeVector mSceneNodeVec;
		LooseOctreeZonePtr mParentZone;
		LooseOctreeZonePtr mChildZones[2][2][2];

	private:
		DISALLOW_COPY_AND_ASSIGN(LooseOctreeZone);
	};

}

#endif